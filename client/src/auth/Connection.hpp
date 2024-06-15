//
// Created by cx9ps3 on 03.06.2024.
//

#pragma once
#include "../http/HttpRequest.hpp"
#include "boost/asio.hpp"
#include "boost/beast.hpp"
#include "boost/beast/websocket.hpp"
#include "rsa.h"
#include <iostream>

namespace auth
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
        bool encrypt(const std::string &data, std::string &encryptedData);

    public:
        Connection(const std::string &newIp, const std::string &newPort);
        bool connectToServer();
        template<rest::Method method, typename... Args>
        void sendRequest(const std::string &body, Args &&...args)
        {
            if constexpr (method == rest::Method::POST || method == rest::Method::PATCH)
            {
                std::string encryptedData;
                encrypt(body, encryptedData);
                rest::HttpRequest<method> request(std::forward<Args>(args)..., encryptedData);
                boost::beast::http::write(socket, *request.getBeastRequestObject());
            }
            else
            {
                rest::HttpRequest<method> request(std::forward<Args>(args)...);
                boost::beast::http::write(socket, *request.getBeastRequestObject());
            }
        }
        boost::beast::http::response<boost::beast::http::dynamic_body> readResponse();
        std::string getIP();
    };
}// namespace auth
