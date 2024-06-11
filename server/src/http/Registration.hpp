//
// Created by cx9ps3 on 03.06.2024.
//

#pragma once
#include "IRoute.hpp"

namespace rest
{
    class Registration : public IRoute
    {
    private:
        struct UserRegistrationData
        {
            long long id;
            std::string login;
            std::string email;
            std::string password;
        };
        std::string decryptedBody;
        bool decryptBody(const server::Connection &clientConnection, const Request &request);
        UserRegistrationData parseBody(const server::Connection &clientConnection,Response &response,const Request &request);

    public:
        Response handleRequest(const server::Connection &clientConnection, const Request &request) override;
    };
}

