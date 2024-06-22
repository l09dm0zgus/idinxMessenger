//
// Created by cx9ps3 on 6/16/24.
//

#include "Login.hpp"
#include "Connection.hpp"
#include <sstream>

std::string auth::Login::serializeAccountData(const std::string &login, const std::string &password)
{
    boost::json::object obj;
    obj["user_data"] =
            {
                    {"login", login},
                    {"password", password}
            };
    std::stringstream ss;
    ss << obj << '\n';
    return ss.str();
}

auth::Login::Login(const std::shared_ptr<Connection> &clientConnection) : connection(clientConnection)
{
}

boost::json::value auth::Login::signInAccount(const std::string &login, const std::string &password)
{
    auto serializedAccountData = serializeAccountData(login, password);
    connection->sendRequest<rest::Method::POST>(serializedAccountData, "/login", connection->getIP(), "application/json");
    return boost::json::parse(connection->readResponse());
}
