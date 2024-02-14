#include "tcp_server/TcpServer.hpp"
#include <iostream>
#include "utils/Logger.hpp"
#include "config/ConfigManager.hpp"
#include "config/ServerConfig.hpp"

int main()
{
    try
    {
        
        utils::Logger::initialize();
        config::ConfigManager::initialize();
        config::ConfigManager::addConfig<config::ServerConfig>();
        server::TCPServer tcpServer;
        tcpServer.run();
    }
    catch (std::exception &e)
    {
        BOOST_LOG_TRIVIAL(error) << "Exception : " << e.what();
    }

    return 0;
}
