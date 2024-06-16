//
// Created by cx9ps3 on 6/16/24.
//

#pragma once
#include <boost/json.hpp>
#include <memory>

namespace auth
{
    class Connection;
    class Login
    {
    private:
        std::shared_ptr<Connection> connection;
        std::string serializeAccountData(const std::string &login, const std::string &password);

    public:
        explicit Login(const std::shared_ptr<Connection> &clientConnection);
        boost::json::value signInAccount(const std::string &login, const std::string &password);
    };
}// namespace auth
