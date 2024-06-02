//
// Created by cx9ps3 on 06.02.2024.
//

#pragma once
#include <array>
#include <boost/asio.hpp>
#include <boost/system.hpp>
#include "../http/Router.hpp"
#include <rsa.h>
#include <osrng.h>

namespace server
{
    class TCPConnection : public std::enable_shared_from_this<TCPConnection>
    {
    public:
        explicit TCPConnection(boost::asio::ip::tcp::socket &&socket, const std::shared_ptr<rest::Router> &newRouter);
        void start();

        static constexpr int RSA_KEY_LENGTH = 2048;
    private:
        std::shared_ptr<rest::Router> router;
        rest::Request request;
        boost::beast::tcp_stream stream;
        boost::beast::flat_buffer buffer;
        CryptoPP::AutoSeededRandomPool randomNumberGenerator;
        CryptoPP::InvertibleRSAFunction parameters;
        std::shared_ptr<CryptoPP::RSA::PublicKey> publicKey;
        std::shared_ptr<CryptoPP::RSA::PrivateKey> privateKey;

        void handleWrite(const boost::system::error_code &error, std::size_t bytesTransferred, const rest::Response &response);
        void handleRead();
        void sendRSAPublicKey();

        void close();
    };
}// namespace server
