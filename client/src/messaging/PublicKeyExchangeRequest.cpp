//
// Created by cx9ps3 on 6/18/24.
//

#include "PublicKeyExchangeRequest.hpp"
#include "../auth/Connection.hpp"
#include "../database/SQLiteDatabase.hpp"
#include "../http/HttpRequest.hpp"
#include <boost/beast.hpp>
#include <sstream>

messaging::PublicKeyExchangeRequest::PublicKeyExchangeRequest(const std::shared_ptr<auth::Connection> &newConnection) : connection(newConnection)
{
    generateKeys();
}

void messaging::PublicKeyExchangeRequest::generateKeys()
{
    parameters.GenerateRandomWithKeySize(randomNumberGenerator, RSA_KEY_LENGTH);
    publicKey = std::make_shared<CryptoPP::RSA::PublicKey>(parameters);
    privateKey = std::make_shared<CryptoPP::RSA::PrivateKey>(parameters);
}

std::shared_ptr<CryptoPP::RSA::PublicKey> messaging::PublicKeyExchangeRequest::getPublicKey()
{
    return publicKey;
}

std::shared_ptr<CryptoPP::RSA::PrivateKey> messaging::PublicKeyExchangeRequest::getPrivateKey()
{
    return privateKey;
}

std::string messaging::PublicKeyExchangeRequest::serializeKeyRequestData(long long senderID, const std::string &receiverLogin)
{
    boost::json::object obj;
    obj["request_key"] =
            {
                    {"sender_id", senderID},
                    {"receiver_login", receiverLogin},
                    {"public_key", rsaKeyToString(publicKey)}};
    std::stringstream ss;
    ss << obj << "\n";
    return ss.str();
}

boost::json::value messaging::PublicKeyExchangeRequest::createRequest(long long int senderID, const std::string &receiverLogin)
{
    savePrivateKeyToDatabase(receiverLogin);
    auto serializedRequestData = serializeKeyRequestData(senderID, receiverLogin);
    connection->disableEncryptionForNextRequest();
    connection->sendRequest<rest::Method::POST>(serializedRequestData, "/keyExchangeRequest", connection->getIP(), "application/json");
    return boost::json::parse(connection->readResponse());
}

void messaging::PublicKeyExchangeRequest::savePrivateKeyToDatabase(const std::string &receiverLogin)
{
    db::SQLiteDatabase database;
    try
    {
        auto deleteQuery = database.query("DELETE FROM key_exchange_requests WHERE receiver_login=?");
        deleteQuery.bind(1, receiverLogin);
        deleteQuery.exec();

        auto insertQuery = database.query("INSERT INTO key_exchange_requests (receiver_login,private_key) VALUES (?,?)");
        insertQuery.bind(1, receiverLogin);
        insertQuery.bind(2, rsaKeyToString(privateKey));
        insertQuery.exec();
    }
    catch (SQLite::Exception &ex)
    {
        std::cout << "Error: " << ex.what() << "\n";
        exit(-1488);
    }
}
