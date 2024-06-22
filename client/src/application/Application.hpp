//
// Created by cx9ps3 on 02.06.2024.
//

#pragma once
#include <iostream>
#include <memory>
#include <boost/thread.hpp>

namespace auth
{
    class Connection;
}
namespace app
{
    class Application
    {
    private:
        std::string getInput(const std::string_view &description);
        void showHelp();
        void connectToServer();
        void startMessaging();
        void loadMessages(const std::string &anotherUserLogin);
        void registerNewAccount();
        void sendRequestForKeyExchange();
        void signInAccount();
        void authenticateUser();
        void addLoginAndPasswordToDatabase(const std::string &login, const std::string password);
        bool isRunning = true;
        bool isAuthenticatedUser = false;
        std::shared_ptr<auth::Connection> connection;
        long long userID;
        std::string userLogin;

    public:
        Application();
        void run();
    };
}// namespace app
