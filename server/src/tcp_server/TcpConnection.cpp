//
// Created by cx9ps3 on 06.02.2024.
//

#include "TcpConnection.hpp"
#include "../utils/Logger.hpp"
#include <iostream>
#include <rsa.h>

void server::TCPConnection::start()
{
    sendRSAPublicKey();
}

void server::TCPConnection::handleRead()
{
    request = {};
    auto self(shared_from_this());
    boost::beast::http::async_read(stream, buffer, request, strand.wrap([this, self](const boost::system::error_code &errorCode, std::size_t bytesTransferred) {
        if (errorCode)
        {
            BOOST_LOG_TRIVIAL(error) << errorCode.message();
        }
        else
        {
            handleWrite(errorCode, bytesTransferred, router->handleRequest(self, request));
        }
    }));
}

void server::TCPConnection::handleWrite([[maybe_unused]] const boost::system::error_code &error, [[maybe_unused]] std::size_t bytesTransferred, std::shared_ptr<rest::Response> response)
{
    auto self(shared_from_this());
    boost::beast::http::async_write(stream, *response, strand.wrap([this, self, response](const boost::system::error_code &errorCode, [[maybe_unused]] std::size_t bytesTransferred) {
        if (errorCode)
        {
            BOOST_LOG_TRIVIAL(error) << errorCode.message();
        }
        else
        {
            handleRead();
        }
    }));
}

server::TCPConnection::TCPConnection(boost::asio::ip::tcp::socket &&socket, const std::shared_ptr<rest::Router> &newRouter, boost::asio::io_context::strand &newStrand) : router(newRouter), stream(std::move(socket)), strand(newStrand)
{
    parameters.GenerateRandomWithKeySize(randomNumberGenerator, RSA_KEY_LENGTH);
    publicKey = std::make_shared<CryptoPP::RSA::PublicKey>(parameters);
    privateKey = std::make_shared<CryptoPP::RSA::PrivateKey>(parameters);
}

void server::TCPConnection::close()
{
    boost::beast::error_code ec;
    stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
}

void server::TCPConnection::sendRSAPublicKey()
{
    CryptoPP::ByteQueue byteQueue;
    publicKey->Save(byteQueue);

    std::array<std::byte, RSA_KEY_LENGTH> bytes{};
    for (int i = 0; i < byteQueue.CurrentSize(); i++)
    {
        bytes[i] = static_cast<std::byte>(byteQueue[i]);
    }

    auto self(shared_from_this());
    boost::asio::async_write(stream.socket(), boost::asio::buffer(bytes, byteQueue.CurrentSize()), strand.wrap([this, self](const boost::system::error_code &errorCode, [[maybe_unused]] std::size_t bytesTransferred) {
        if (errorCode)
        {
            BOOST_LOG_TRIVIAL(error) << errorCode.message();
        }
        else
        {
            handleRead();
        }
    }));
}

void server::TCPConnection::setID(long long int newID)
{
    id = newID;
}

long long server::TCPConnection::getID() const noexcept
{
    return id;
}

bool server::TCPConnection::decrypt(const std::string &encryptedData, std::string &decryptedData)
{
    auto isSuccessfulDecrypted = false;
    CryptoPP::RSAES_OAEP_SHA_Decryptor decryptor(*privateKey);

    try
    {
        auto stringSink = new CryptoPP::StringSink(decryptedData);
        auto decryptorFilter = new CryptoPP::PK_DecryptorFilter(randomNumberGenerator, decryptor, stringSink);
        CryptoPP::StringSource ss2(encryptedData, true, decryptorFilter);
        isSuccessfulDecrypted = true;
    }
    catch (const CryptoPP::Exception &ex)
    {
        BOOST_LOG_TRIVIAL(error) << ex.what() << "\n";
    }

    return isSuccessfulDecrypted;
}

void server::TCPConnection::send(const std::string_view &data)
{
    std::array<char,MESSAGE_LENGTH> array{};
    std::copy(data.begin(),data.end(),array.begin());
    boost::asio::write(stream.socket(), boost::asio::buffer(array, data.size()));
}
