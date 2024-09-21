//
// Created by cx9ps3 on 6/17/24.
//

#pragma once
#include "IRoute.hpp"

namespace rest
{
    class MessageReceiver : public IRoute
    {
    private:
        std::string login;
        std::string encryptedPassword;
        std::string decryptedPassword;
        std::string allUserMessages;
        long long userID{0};
        bool parseQuery(std::shared_ptr<Response> &response, const Request &request);
        bool decryptUserPassword(const server::Connection &clientConnection,const Request &request);
        bool checkIfLoginExistsInDatabase(std::shared_ptr<Response> &response, const Request &request);
        bool checkIfPasswordExistsInDatabase(std::shared_ptr<Response> &response, const Request &request);
        bool getAllMessagesForUser(std::shared_ptr<Response> &response, const Request &request);
        std::shared_ptr<Response> createResponseWithMessages(const Request &request);

    public:
        MessageReceiver() = default;
        std::shared_ptr<Response> handleRequest(const server::Connection &clientConnection, const Request &request) override;
    };
}

