#include "tcp_server/TcpServer.hpp"
#include <iostream>

int main()
{
    try
    {
        server::TCPServer tcpServer;
        tcpServer.run();
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
