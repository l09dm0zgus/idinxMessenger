#include "tcp_server/TcpServer.hpp"
#include <iostream>
#include "utils/Logger.hpp"

int main()
{
    try
    {
        utils::Logger::initialize();
        server::TCPServer tcpServer;
        tcpServer.run();
    }
    catch (std::exception &e)
    {
        BOOST_LOG_TRIVIAL(error) << "Exception : " << e.what();
    }

    return 0;
}
