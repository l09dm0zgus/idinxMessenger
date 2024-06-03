//
// Created by cx9ps3 on 03.06.2024.
//

#pragma once
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/websocket.hpp>
#include <iostream>
#include <rsa.h>

namespace app
{
    using boost::asio::ip::tcp;
    class Connection
    {
    private:
        bool isSuccessfulConnected = false;
        boost::asio::io_context ioContext;
        tcp::socket socket;
        tcp::resolver resolver;
        std::string ip;
        std::string port;
        CryptoPP::RSA::PublicKey publicKey;
        static constexpr int RSA_KEY_LENGTH = 2048;
        void readRSAPublicKey();

    public:
        Connection(const std::string &newIp, const std::string &newPort);
        bool connectToServer();
    };
}// namespace app
