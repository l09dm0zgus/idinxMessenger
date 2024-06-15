//
// Created by cx9ps3 on 03.06.2024.
//

#include "Connection.hpp"
#include "../http/HttpRequest.hpp"
#include "osrng.h"


auth::Connection::Connection(const std::string &newIp, const std::string &newPort) : socket(ioContext), resolver(ioContext), ip(newIp), port(newPort)
{
}

bool auth::Connection::connectToServer()
{
    auto endpoint = *resolver.resolve(ip, port).begin();

    try
    {
        socket.connect(endpoint);
        isSuccessfulConnected = true;
        readRSAPublicKey();
    }
    catch (boost::system::error_code &ex)
    {
        std::cout << "Failed to connect!\n What: " << ex.what() << "\n";
    }

    return isSuccessfulConnected;
}

void auth::Connection::readRSAPublicKey()
{
    std::array<std::byte, RSA_KEY_LENGTH> bytes{};
    boost::system::error_code errorCode;
    auto readBytes = socket.read_some(boost::asio::buffer(bytes));

    if (readBytes <= 0)
    {
        std::cout << "Failed to get public RSA key!\n";
    }

    if (errorCode)
    {
        std::cout << errorCode.message() << "\n";
        exit(-1488);
    }
    else
    {
        CryptoPP::ByteQueue queue;
        for (int i = 0; i < readBytes; i++)
        {
            queue.Put(static_cast<CryptoPP::byte>(bytes[i]));
        }

        publicKey.Load(queue);
    }
}

bool auth::Connection::encrypt(const std::string &data, std::string &encryptedData)
{
    auto isSuccessfulEncrypted = false;
    CryptoPP::RSAES_OAEP_SHA_Encryptor encryptor(publicKey);
    CryptoPP::AutoSeededRandomPool rng;
    try
    {
        auto stringSink = new CryptoPP::StringSink(encryptedData);
        auto encryptorFilter = new CryptoPP::PK_EncryptorFilter(rng, encryptor, stringSink);
        CryptoPP::StringSource ss2(data, true, encryptorFilter);
    }
    catch (const CryptoPP::Exception &ex)
    {
        std::cout << "Failed to encrypt:" << ex.what() << "\n";
    }

    return isSuccessfulEncrypted;
}
std::string auth::Connection::getIP()
{
    return ip;
}

boost::beast::http::response<boost::beast::http::dynamic_body> auth::Connection::readResponse()
{
    boost::beast::flat_buffer buffer;
    boost::beast::http::response<boost::beast::http::dynamic_body> response;
    boost::beast::http::read(socket, buffer, response);
    return response;
}
