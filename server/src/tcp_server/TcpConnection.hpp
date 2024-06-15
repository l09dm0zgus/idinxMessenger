//
// Created by cx9ps3 on 06.02.2024.
//

#pragma once
#include "../http/Router.hpp"
#include <array>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/system.hpp>
#include <osrng.h>
#include <rsa.h>

namespace server
{
    class TCPConnection : public std::enable_shared_from_this<TCPConnection>
    {
    public:
        explicit TCPConnection(boost::asio::ip::tcp::socket &&socket, const std::shared_ptr<rest::Router> &newRouter, boost::asio::io_context::strand &newStrand);

        bool decrypt(const std::string &encryptedData, std::string &decryptedData);
        void setID(long long newID);
        long long getID() const noexcept;
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
        boost::asio::io_context::strand &strand;
        long long id{0};

        void handleWrite([[maybe_unused]] const boost::system::error_code &error, [[maybe_unused]] std::size_t bytesTransferred, std::shared_ptr<rest::Response> response);
        void handleRead();
        void sendRSAPublicKey();

        void close();
    };
}// namespace server
