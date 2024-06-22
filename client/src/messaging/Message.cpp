//
// Created by cx9ps3 on 6/18/24.
//

#include "Message.hpp"
#include "../http/HttpRequest.hpp"
#include "../auth/Connection.hpp"
#include "modes.h"
#include <hex.h>
#include <osrng.h>
#include <rijndael.h>
#include <sstream>


messaging::Message::Message(const std::shared_ptr<auth::Connection> &newConnection,const std::string &newReceiverLogin, const std::string &newSenderLogin ,long long newSenderID) : receiverLogin(newReceiverLogin), senderID(newSenderID),connection(newConnection),senderLogin(newSenderLogin)
{
}

std::string messaging::Message::encryptUserMessage(const std::string &userMessage)
{
    CryptoPP::AutoSeededRandomPool prng;
    std::string encodedString;
    CryptoPP::HexEncoder encoder(new CryptoPP::StringSink(encodedString));

    CryptoPP::SecByteBlock key(CryptoPP::AES::DEFAULT_KEYLENGTH);
    CryptoPP::SecByteBlock iv(CryptoPP::AES::BLOCKSIZE);

    prng.GenerateBlock(key, key.size());
    prng.GenerateBlock(iv, iv.size());

    encoder.Put(iv,iv.size());
    encoder.MessageEnd();
    aesKey = encodedString + "$";

    encodedString = "";
    encoder.Put(key,key.size());
    encoder.MessageEnd();
    aesKey = aesKey + encodedString;
    std::string cipher;
    try
    {
        CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption e;
        e.SetKeyWithIV(key, key.size(), iv);
        CryptoPP::StringSource s(userMessage, true,new CryptoPP::StreamTransformationFilter(e,new CryptoPP::StringSink(cipher)));
    }
    catch(const CryptoPP::Exception& e)
    {
        std::cout << "Failed to encrypt message: " << e.what() << "\n";
        exit(-1488);
    }

    encodedString = "";
    encoder.Put(reinterpret_cast<CryptoPP::byte*>(cipher.data()),cipher.size());
    encoder.MessageEnd();
    return encodedString;
}

std::string messaging::Message::encryptAESKey()
{
    auto key = getLastKeyFromDatabase<CryptoPP::RSA::PublicKey>(receiverLogin);
    CryptoPP::RSAES_OAEP_SHA_Encryptor encryptor(*key);
    std::string encryptedKey;
    CryptoPP::AutoSeededRandomPool rng;
    try
    {
        auto stringSink = new CryptoPP::StringSink(encryptedKey);
        auto encryptorFilter = new CryptoPP::PK_EncryptorFilter(rng, encryptor, stringSink);
        CryptoPP::StringSource ss2(aesKey, true, encryptorFilter);
    }
    catch (const CryptoPP::Exception &ex)
    {
        std::cout << "Failed to encrypt key:" << ex.what() << "\n";
        exit(-1488);
    }

    std::string hexEncodedString;
    CryptoPP::HexEncoder encoder(new CryptoPP::StringSink(hexEncodedString));

    encoder.Put(reinterpret_cast<CryptoPP::byte*>(encryptedKey.data()),encryptedKey.size());
    encoder.MessageEnd();
    return hexEncodedString;
}

std::string messaging::Message::serializeUserMessage(const std::string &userMessage)
{
    boost::json::object obj;
    obj["user_message"] =
            {
                    {"sender_id", senderID},
                    {"receiver_login", receiverLogin},
                    {"public_key", PublicKeyExchangeRequest::rsaKeyToString(nextPublicKey)},
                    {"message", encryptUserMessage(userMessage)},
                    {"key",encryptAESKey()}
            };
    std::stringstream ss;
    ss << obj << "\n";
    return ss.str();
}

auto messaging::Message::generateNextRSAKeys()
{
    CryptoPP::AutoSeededRandomPool randomNumberGenerator;
    CryptoPP::InvertibleRSAFunction parameters;
    parameters.GenerateRandomWithKeySize(randomNumberGenerator, PublicKeyExchangeRequest::RSA_KEY_LENGTH);
    auto nextPublicKey = std::make_shared<CryptoPP::RSA::PublicKey>(parameters);
    auto nextPrivateKey = std::make_shared<CryptoPP::RSA::PrivateKey>(parameters);

    return std::make_pair(nextPublicKey,nextPrivateKey);
}


boost::json::value messaging::Message::send(const std::string &userMessage)
{
    auto keys = generateNextRSAKeys();
    nextPublicKey = keys.first;
    nextPrivateKey = keys.second;

    storeToDatabase(userMessage);
    
    auto encryptedUserMessage = serializeUserMessage(userMessage);
    connection->disableEncryptionForNextRequest();
    connection->sendRequest<rest::Method::POST>(encryptedUserMessage, "/sendMessage", connection->getIP(), "application/json");
    return boost::json::parse(connection->readResponse());
}

void messaging::Message::storeToDatabase(const std::string &userMessage)
{
    db::SQLiteDatabase database;
    try
    {
        auto selectQuery = database.query("SELECT conversation_id FROM messages WHERE (receiver=? AND sender=?) OR (sender=? AND receiver=?)");
        selectQuery.bind(1,receiverLogin);
        selectQuery.bind(2,senderLogin);
        selectQuery.bind(3,receiverLogin);
        selectQuery.bind(4,senderLogin);

        long long conservationID;
        if(selectQuery.executeStep())
        {
            conservationID = selectQuery.getColumn("conversation_id").getInt64();
        }
        else
        {
            CryptoPP::AutoSeededRandomPool rng;
            CryptoPP::Integer id(rng, 0, std::numeric_limits<long>::max());
            conservationID = id.ConvertToLong();
        }


        auto insertQuery = database.query("INSERT INTO messages (sender,receiver,message,conversation_id,next_private_key) VALUES (?,?,?,?,?)");
        insertQuery.bind(1, senderLogin);
        insertQuery.bind(2, receiverLogin);
        insertQuery.bind(3,userMessage);
        insertQuery.bind(4,static_cast<int64_t>(conservationID));
        insertQuery.bind(5,PublicKeyExchangeRequest::rsaKeyToString(nextPrivateKey));
        insertQuery.exec();
    }
    catch (SQLite::Exception &ex)
    {
        std::cout << "Error: " << ex.what() << "\n";
        exit(-1488);
    }

}

std::string messaging::Message::decryptAESKey(const std::string_view &encryptedAESKey, const std::string_view &senderLogin)
{
    auto privateKey = getLastKeyFromDatabase<CryptoPP::RSA::PrivateKey>(senderLogin.data());
    std::string decryptedKey;
    std::string decodedKey;
    CryptoPP::StringSource s1(encryptedAESKey.data(), true,new CryptoPP::HexDecoder(new CryptoPP::StringSink(decodedKey)));

    CryptoPP::RSAES_OAEP_SHA_Decryptor decryptor(*privateKey);

    try
    {
        CryptoPP::AutoSeededRandomPool randomNumberGenerator;
        auto stringSink = new CryptoPP::StringSink(decryptedKey);
        auto decryptorFilter = new CryptoPP::PK_DecryptorFilter(randomNumberGenerator, decryptor, stringSink);
        CryptoPP::StringSource ss2(decodedKey, true, decryptorFilter);
    }
    catch (const CryptoPP::Exception &ex)
    {
        std::cout << ex.what() << "\n";
    }

    return decryptedKey;
}
std::string messaging::Message::decryptMessage(const std::string_view &encryptedMessage, const std::string &aesKey)
{
    std::stringstream ss(aesKey);
    std::string str;
    std::vector<std::string> strings;
    while (std::getline(ss,str,'$'))
    {
        strings.push_back(str);
    }

    std::string iv,key,decodedString;

    CryptoPP::StringSource s1(strings[0], true,new CryptoPP::HexDecoder(new CryptoPP::StringSink(iv)));
    CryptoPP::StringSource s2(strings[1], true,new CryptoPP::HexDecoder(new CryptoPP::StringSink(key)));
    CryptoPP::StringSource s3(encryptedMessage.data(), true,new CryptoPP::HexDecoder(new CryptoPP::StringSink(decodedString)));
    std::string decryptedMessage;

    try
    {
        CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption d;
        d.SetKeyWithIV(reinterpret_cast<CryptoPP::byte*>(key.data()), key.size(), reinterpret_cast<CryptoPP::byte*>(iv.data()));

        CryptoPP::StringSource s(decodedString.data(), true,new CryptoPP::StreamTransformationFilter(d,new CryptoPP::StringSink(decryptedMessage)));
    }
    catch(const CryptoPP::Exception& e)
    {
        std::cout << e.what() << "\n";
        exit(-1488);
    }
    return decryptedMessage;
}

std::string messaging::Message::decryptMessage(const std::string_view &encryptedMessage, const std::string_view &encryptedAESKey, const std::string_view &senderLogin)
{
    return decryptMessage(encryptedMessage, decryptAESKey(encryptedAESKey,senderLogin));
}
