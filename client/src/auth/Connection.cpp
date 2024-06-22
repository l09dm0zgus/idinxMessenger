//
// Created by cx9ps3 on 03.06.2024.
//

#include "Connection.hpp"
#include "../http/HttpRequest.hpp"
#include "../messaging/MessageParser.hpp"
#include <iostream>
#include <mutex>
#include <osrng.h>
#include <thread>

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

std::string auth::Connection::readResponse()
{
    while (isLastResponseReaded) {};
    std::cout << lastResponse << "\n";
    return lastResponse;
}

void auth::Connection::disableEncryptionForNextRequest()
{
    isEncryptionEnabled = false;
}

void auth::Connection::handleRead()
{
    response = {};
    auto self(shared_from_this());
    isLastResponseReaded = true;
    boost::beast::http::async_read(socket, buffer, response, [this, self](const boost::system::error_code &errorCode, std::size_t bytesTransferred) {
        if (errorCode)
        {
            std::cout  << errorCode.message();
        }
        else
        {
            handleParseMessage(errorCode, bytesTransferred,self);
        }
    });
}

void auth::Connection::handleParseMessage([[maybe_unused]] const boost::system::error_code &errorCode,[[maybe_unused]] std::size_t bytesTransferred, const std::shared_ptr<Connection> &connection)
{
    auto exchangeKeyCallback = [](const std::string_view &receiver, const std::string_view &sender, const std::string_view publicReceiverKey) {
        std::cout << "User " << sender << " want exchange keys with you\n";
        return true;
    };

    auto sentMessageCallback = [connection](const std::string_view &senderLogin,const std::string_view& message)
    {
        connection->bIsHaveNewMessages = true;
        std::cout << "You have new message from: " << senderLogin << "\n";
    };

    auto jsonString = boost::beast::buffers_to_string(response.body().data());
    auto jsonObject = boost::json::parse(jsonString).as_object();
    if(jsonObject.if_contains("response"))
    {
        std::cout << jsonString << "\n";
        lastResponse = jsonString;
    }
    else
    {
        messaging::MessageParser parser(connection, connectionID, exchangeKeyCallback,sentMessageCallback);
        parser.parseJSON(jsonString);
    }
    isLastResponseReaded = false;

    handleRead();
}

void auth::Connection::startReceivingAsyncMessages()
{
    handleRead();
    auto thread = std::thread([this]() {ioContext.run();});
    thread.detach();
}

void auth::Connection::setID(long long newConnectionID)
{
    connectionID = newConnectionID;
}

bool auth::Connection::isHasNewMessages()
{
    if (bIsHaveNewMessages)
    {
        bIsHaveNewMessages = false;
        return true;
    }
    else
    {
        return false;
    }
}
