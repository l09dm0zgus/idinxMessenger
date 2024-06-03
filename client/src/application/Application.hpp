//
// Created by cx9ps3 on 02.06.2024.
//

#pragma once
#include "Connection.hpp"
#include <iostream>

namespace app
{
    class Application
    {
    private:
        std::string getInput(const std::string_view &description);
        void showHelp();
        void connectToServer();
        bool isRunning = true;
        std::unique_ptr<Connection> connection;

    public:
        Application();
        void run();
    };
}// namespace app
