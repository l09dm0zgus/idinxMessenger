#include "tcp_server/TcpServer.hpp"
#include <iostream>
#include "utils/Logger.hpp"
#include "config/ConfigManager.hpp"
#include "config/ServerConfig.hpp"
#include "config/DatabaseConfig.hpp"

int main()
{
    try
    {
        utils::Logger::initialize();
        config::ConfigManager::initialize();
        config::ConfigManager::addConfig<config::ServerConfig>();
        config::ConfigManager::addConfig<config::DatabaseConfig>();
        server::TCPServer tcpServer;
        tcpServer.run();
    }
    catch (std::exception &e)
    {
        BOOST_LOG_TRIVIAL(error) << "Exception : " << e.what();
    }

    return 0;
}
