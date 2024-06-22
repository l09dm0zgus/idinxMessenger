//
// Created by cx9ps3 on 02.06.2024.
//

#include "Application.hpp"
#include "../auth/Connection.hpp"
#include "../auth/Login.hpp"
#include "../auth/Registration.hpp"
#include "../database/SQLiteDatabase.hpp"
#include "../messaging/Message.hpp"
#include "../messaging/PublicKeyExchangeRequest.hpp"

std::string app::Application::getInput(const std::string_view &description)
{
    std::cout << description;
    std::string input;
    std::cin >> input;
    return input;
}

void app::Application::run()
{
    std::cout << "Welcome to messenger.Enter help to show available commands\n";

    while (isRunning)
    {
        const auto command = getInput("Command:");

        if (command == "exit")
        {
            std::cout << "Bye:(\n";
            isRunning = false;
        }
        else if (command == "help")
        {
            showHelp();
        }
        else if (command == "connect")
        {
            connectToServer();
        }
        else if(command == "request")
        {
            sendRequestForKeyExchange();
        }
        else if(command == "message")
        {
            startMessaging();
        }
        else
        {
            std::cout << "Unknown command.See command - help\n";
        }
    }
}

void app::Application::showHelp()
{
    std::cout << "help - show this menu.\n";
    std::cout << "exit - exit from CLI messenger client.\n";
    std::cout << "connect - connect to messenger server.\n";
    std::cout << "message - go to  correspondence with user.\n";
    std::cout << "request - send request to user for messaging\n";
}

app::Application::Application()
{
}

void app::Application::connectToServer()
{
    const auto ip = getInput("Ip:");
    const auto port = getInput("Port:");
    connection = std::make_shared<auth::Connection>(ip, port);

    if (connection->connectToServer())
    {
        std::cout << "Connected to : " << ip << ":" << port << "\n";
        authenticateUser();
    }
}

void app::Application::registerNewAccount()
{
    auth::Registration::AccountData accountData;
    bool isContinueRegistration = true;
    while (isContinueRegistration)
    {
        std::cout << "Login: ";
        std::cin >> accountData.login;

        std::cout << "Password: ";
        std::cin >> accountData.password;

        std::cout << "Email: ";
        std::cin >> accountData.email;

        auth::Registration reg(connection);
        auto result = reg.registerNewAccount(accountData);
        auto code = result.at("response").at("code").as_int64();
        if (code == 1)
        {
            userLogin = accountData.login;
            addLoginAndPasswordToDatabase(accountData.login, accountData.password);
            userID = result.at("response").at("session_id").as_int64();
            std::cout << "Successful registration:)\n";
            isContinueRegistration = false;
            isAuthenticatedUser = true;
            connection->setID(userID);
        }
        else
        {
            std::cout << "Error: " << result.at("response").at("what");
            std::cout << "Do you want try again register?( 1 - no, anything else - yes: ";
            int answer = 0;
            std::cin >> answer;
            if (answer == 1)
            {
                isContinueRegistration = false;
                isRunning = false;
            }
        }
    }
}

void app::Application::authenticateUser()
{
    db::SQLiteDatabase database;
    connection->startReceivingAsyncMessages();
    try
    {
        auto query = database.query("SELECT * FROM users;");
        if (query.executeStep())
        {
            auth::Login log(connection);
            userLogin = query.getColumn("login").getString();
            auto result = log.signInAccount(userLogin, query.getColumn("password").getString());
            auto code = result.at("response").at("code").as_int64();
            if (code == 1)
            {
                userID = result.at("response").at("session_id").as_int64();
                std::cout << "Successful sign-up:)\n";
                isAuthenticatedUser = true;
                connection->setID(userID);
            }
            else
            {
                std::cout << "Error: " << result.at("response").at("what");
                isRunning = false;
            }
        }
        else
        {
            std::cout << "Do you want create account or login?\n";
            std::cout << "1 - login,2 - create\n";
            int option = 0;
            while (option != 1 || option != 2)
            {
                std::cout << "Your answer:";
                std::cin >> option;

                if (option == 1)
                {
                    signInAccount();
                    break;
                }
                else if (option == 2)
                {
                    registerNewAccount();
                    break;
                }
            }
        }
    }
    catch (SQLite::Exception &ex)
    {
        std::cout << "Error :" << ex.what() << "\n";
    }
}

void app::Application::signInAccount()
{
    bool isContinueLogin = true;
    std::string login, password;
    while (isContinueLogin)
    {
        std::cout << "Login: ";
        std::cin >> login;

        std::cout << "Password: ";
        std::cin >> password;

        auth::Login log(connection);
        auto result = log.signInAccount(login, password);
        auto code = result.at("response").at("code").as_int64();
        if (code == 1)
        {
            userLogin = login;
            addLoginAndPasswordToDatabase(login, password);
            userID = result.at("response").at("session_id").as_int64();
            std::cout << "Successful sign-up:)\n";
            isContinueLogin = false;
            isAuthenticatedUser = true;
            connection->setID(userID);
        }
        else
        {
            std::cout << "Error: " << result.at("response").at("what");
            std::cout << "Do you want try again sign-up?( 1 - no, anything else - yes: ";
            int answer = 0;
            std::cin >> answer;
            if (answer == 1)
            {
                isContinueLogin = false;
                isRunning = false;
            }
        }
    }
}

void app::Application::addLoginAndPasswordToDatabase(const std::string &login, const std::string password)
{
    db::SQLiteDatabase database;
    try
    {
        auto query = database.query("INSERT INTO users VALUES (1,?,?)");
        query.bind(1, login);
        query.bind(2, password);
        query.exec();
    }
    catch (SQLite::Exception &ex)
    {
        std::cout << "Error: " << ex.what() << "\n";
        exit(-1488);
    }
}

void app::Application::sendRequestForKeyExchange()
{
    if(isAuthenticatedUser)
    {
        std::string receiverLogin;
        std::cout << "Please write user nickname for request: ";
        std::cin >> receiverLogin;

        if(receiverLogin == userLogin)
        {
            std::cout << "You cannot exchange keys with yourself!\n";
            return;
        }
        db::SQLiteDatabase database;
        auto query = database.query("SELECT * FROM key_exchange_requests WHERE private_key NOT NULL AND receiver_public_key NOT NULL AND receiver_login=?");
        query.bind(1,receiverLogin);
        if(query.executeStep())
        {
            std::cout << "You have already exchanged keys with this user!\n";
        }
        else
        {
            messaging::PublicKeyExchangeRequest request(connection);
            auto result = request.createRequest(userID,receiverLogin);
            auto code = result.at("response").at("code").as_int64();
            auto what = result.at("response").at("what").as_string();
            std::cout << what << "\n";
        }
    }
    else
    {
        std::cout << "You must first connect to server for exchanging keys!\n";
    }

}

void app::Application::startMessaging()
{
    if(isAuthenticatedUser)
    {
        int users = 0;
        db::SQLiteDatabase database;
        std::vector<std::string> usersLogins;
        auto query = database.query("SELECT * FROM key_exchange_requests WHERE private_key NOT NULL AND receiver_public_key NOT NULL;");
        std::cout << "Please select user which you want messaging (write number).\n";
        while(query.executeStep())
        {
            usersLogins.push_back(query.getColumn("receiver_login"));
            std::cout << users << " - " << query.getColumn("receiver_login") << "\n";
            users++;
        }

        int answer;
        std::cout << "Your answer: ";
        std::cin >> answer;
        if(answer >= 0 && answer < users && users > 0)
        {
            bool isMessaging = true;
            while (isMessaging)
            {
                loadMessages(usersLogins[answer]);
                messaging::Message message(connection,usersLogins[answer],userLogin,userID);

                std::cout << "Your message:";
                std::string msg;
                std::getline(std::cin, msg);

                if(msg == "/q")
                {
                    isMessaging = false;
                    return;
                }


                auto response = message.send(msg);
                auto what = response.at("response").at("what").as_string();
                std::cout << what << "\n";
            }
        }
        else
        {
            std::cout << "Wrong user number!\n";
        }
    }
    else
    {
        std::cout << "For messaging you need first connect to server!\n";
    }
}

void app::Application::loadMessages(const std::string &anotherUserLogin)
{
    db::SQLiteDatabase database;
    auto query = database.query("SELECT conversation_id FROM messages WHERE sender=?");
    query.bind(1,anotherUserLogin);

    if(query.executeStep())
    {
        auto conservationID = query.getColumn("conversation_id").getInt64();
        auto messagesQuery = database.query("SELECT sender , message FROM messages WHERE conversation_id=?");
        messagesQuery.bind(1,conservationID);
        while (messagesQuery.executeStep())
        {
            std::cout << messagesQuery.getColumn("sender").getString() << ":" << messagesQuery.getColumn("message").getString() << "\n";
        }
    }
}
