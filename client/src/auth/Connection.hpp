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
#include <future>
#include <stack>

namespace auth
{
    using boost::asio::ip::tcp;
    class Connection : public std::enable_shared_from_this<Connection>
    {
    private:
        bool isSuccessfulConnected = false;
        boost::asio::io_context ioContext;
        tcp::socket socket;
        tcp::resolver resolver;
        std::string ip;
        std::string port;
        CryptoPP::RSA::PublicKey publicKey;
        boost::beast::flat_buffer buffer;
        boost::beast::http::response<boost::beast::http::dynamic_body> response;
        long long connectionID;
        static constexpr int RSA_KEY_LENGTH = 2048;
        static constexpr int MESSAGE_LENGTH = RSA_KEY_LENGTH + 8128;
        bool isEncryptionEnabled = true;
        bool bIsHaveNewMessages;
        void readRSAPublicKey();
        bool encrypt(const std::string &data, std::string &encryptedData);
        void handleParseMessage([[maybe_unused]] const boost::system::error_code &errorCode,[[maybe_unused]] std::size_t bytesTransferred, const std::shared_ptr<Connection> &connection);
        void handleRead();
        std::string lastResponse;
        std::atomic_bool isLastResponseReaded = false;

    public:
        Connection(const std::string &newIp, const std::string &newPort);
        bool connectToServer();
        void startReceivingAsyncMessages();
        bool isHasNewMessages();
        template<rest::Method method, typename... Args>
        void sendRequest(const std::string &body, Args &&...args)
        {
            if constexpr (method == rest::Method::POST || method == rest::Method::PATCH)
            {
                if (isEncryptionEnabled)
                {
                    std::string encryptedData;
                    encrypt(body, encryptedData);
                    rest::HttpRequest<method> request(std::forward<Args>(args)..., encryptedData);
                    boost::beast::http::write(socket, *request.getBeastRequestObject());
                }
                else
                {
                    rest::HttpRequest<method> request(std::forward<Args>(args)..., body);
                    boost::beast::http::write(socket, *request.getBeastRequestObject());
                    isEncryptionEnabled = true;
                }
            }
            else
            {
                rest::HttpRequest<method> request(std::forward<Args>(args)...);
                boost::beast::http::write(socket, *request.getBeastRequestObject());
            }
        }
        std::string readResponse();
        std::string getIP();
        void disableEncryptionForNextRequest();
        void setID(long long newConnectionID);
    };
}// namespace auth