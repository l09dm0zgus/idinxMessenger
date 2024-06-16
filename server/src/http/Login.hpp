//
// Created by cx9ps3 on 6/15/24.
//

#pragma once
#include "IRoute.hpp"
namespace server
{
    class TCPConnection;
}
namespace rest
{
    class Login : public IRoute
    {
    private:
        std::unordered_map<long long, std::shared_ptr<server::TCPConnection>> &connections;
        bool decryptBody(const server::Connection &clientConnection, const Request &request);
        std::pair<std::string, std::string> parseBody(const server::Connection &clientConnection, std::shared_ptr<Response> &response, const Request &request);
        bool checkIfLoginExistsInDatabase(const std::string &login, std::shared_ptr<Response> &response, const Request &request);
        bool checkIfPasswordExistsInDatabase(const std::string &login, const std::string &password, std::shared_ptr<Response> &response, const Request &request);
        std::string decryptedBody;
        long long userID = 0;

    public:
        std::shared_ptr<Response> handleRequest(const server::Connection &clientConnection, const Request &request) override;
        explicit Login(std::unordered_map<long long, std::shared_ptr<server::TCPConnection>> &newConnections);
    };
}// namespace rest
