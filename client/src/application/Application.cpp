//
// Created by cx9ps3 on 02.06.2024.
//

#include "Application.hpp"
#include "../auth/Connection.hpp"
#include "../auth/Login.hpp"
#include "../auth/Registration.hpp"
#include "../database/SQLiteDatabase.hpp"

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
            addLoginAndPasswordToDatabase(accountData.login, accountData.password);
            userID = result.at("response").at("session_id").as_int64();
            std::cout << "Successful registration:)\n";
            isContinueRegistration = false;
            isAuthenticatedUser = true;
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
    try
    {
        auto query = database.query("SELECT * FROM users;");
        if (query.executeStep())
        {
            auth::Login log(connection);
            auto result = log.signInAccount(query.getColumn("login"), query.getColumn("password"));
            auto code = result.at("response").at("code").as_int64();
            if (code == 1)
            {
                userID = result.at("response").at("session_id").as_int64();
                std::cout << "Successful sign-up:)\n";
                isAuthenticatedUser = true;
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
            addLoginAndPasswordToDatabase(login, password);
            userID = result.at("response").at("session_id").as_int64();
            std::cout << "Successful sign-up:)\n";
            isContinueLogin = false;
            isAuthenticatedUser = true;
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
