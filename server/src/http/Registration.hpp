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
        struct UserRegistrationData {
            long long id;
            std::string login;
            std::string email;
            std::string password;
        };

        std::string decryptedBody;
        bool decryptBody(const server::Connection &clientConnection, const Request &request);
        UserRegistrationData parseBody(const server::Connection &clientConnection, std::shared_ptr<Response> &response, const Request &request);
        bool checkIfLoginExistsInDatabase(const std::string &login, std::shared_ptr<Response> &response, const Request &request);
        bool checkIfEmailExistsInDatabase(const std::string &email, std::shared_ptr<Response> &response, const Request &request);
        void writeRegistrationDataToDatabase(const UserRegistrationData &userRegistrationData, std::shared_ptr<Response> &response, const Request &request);

    public:
        std::shared_ptr<Response> handleRequest(const server::Connection &clientConnection, const Request &request) override;
    };
}// namespace rest
