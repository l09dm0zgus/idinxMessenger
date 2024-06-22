//
// Created by cx9ps3 on 6/17/24.
//

#pragma once
#include "KeyRequester.hpp"

namespace rest
{
    class MessageSender : public KeyRequester
    {
    private:
        bool parseBody(const std::string_view &body,std::shared_ptr<Response> &response,const Request &request);
        std::string createMessage();
        std::string encryptedAESKey;
        std::string encryptedMessage;

    public:
        explicit MessageSender(std::unordered_map<long long, std::shared_ptr<server::TCPConnection>> &newConnections);
        std::shared_ptr<Response> handleRequest(const server::Connection &clientConnection, const Request &request) override;
    };
}

