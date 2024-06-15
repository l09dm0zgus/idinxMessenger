//
// Created by cx9ps3 on 6/14/24.
//

#pragma once
#include <iostream>
#include <memory>

namespace auth
{
    class Connection;
    class Registration
    {
    public:
        struct AccountData {
            std::string login;
            std::string password;
            std::string email;
        };

    private:
        std::shared_ptr<Connection> connection;
        std::string serializeAccountData(const AccountData &accountData);

    public:
        explicit Registration(const std::shared_ptr<Connection> &newConnection);
        std::string registerNewAccount(const AccountData &accountData);
    };
}// namespace auth
