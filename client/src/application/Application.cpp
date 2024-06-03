//
// Created by cx9ps3 on 02.06.2024.
//

#include "Application.hpp"
#include "queue.h"
#include "rsa.h"

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
    connection = std::make_unique<Connection>(ip, port);

    if (!connection->connectToServer())
    {
        std::cout << "Connected to : " << ip << ":" << port << "\n";
    }
}
