//
// Created by cx9ps3 on 6/14/24.
//

#include "Registration.hpp"
#include "Connection.hpp"
#include <sstream>

auth::Registration::Registration(const std::shared_ptr<Connection> &newConnection) : connection(newConnection)
{
}

boost::json::value auth::Registration::registerNewAccount(const auth::Registration::AccountData &accountData)
{
    auto serializedAccountData = serializeAccountData(accountData);
    connection->sendRequest<rest::Method::POST>(serializedAccountData, "/registration", connection->getIP(), "application/json");
    auto response = connection->readResponse();
    return boost::json::parse(boost::beast::buffers_to_string(response.body().data()));
}

std::string auth::Registration::serializeAccountData(const auth::Registration::AccountData &accountData)
{
    boost::json::object obj;
    obj["user_data"] =
            {
                    {"email", accountData.email},
                    {"login", accountData.login},
                    {"password", accountData.password}};
    std::stringstream ss;
    ss << obj << '\n';
    return ss.str();
}
