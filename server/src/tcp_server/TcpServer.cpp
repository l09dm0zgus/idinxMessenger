//
// Created by cx9ps3 on 06.02.2024.
//
#include "TcpServer.hpp"
#include "../utils/Logger.hpp"
#include "../config/ConfigManager.hpp"
#include "../http/Registration.hpp"
#include "../config/ServerConfig.hpp"
#include "TcpConnection.hpp"
#include <iostream>


server::TCPServer::TCPServer() : serverAcceptor(ioContext), signals(ioContext)
{
    boost::asio::ip::tcp::resolver resolver(ioContext);
    strand = std::make_shared<boost::asio::io_context::strand>(ioContext);
    auto hostname = config::ConfigManager::getConfig<config::ServerConfig>()->getHostname();
    auto port = config::ConfigManager::getConfig<config::ServerConfig>()->getPort();

    boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(hostname, std::to_string(port)).begin();

    boost::beast::error_code  errorCode;

    serverAcceptor.open(endpoint.protocol(), errorCode);
    if(errorCode)
    {
        BOOST_LOG_TRIVIAL(error) << errorCode.message();
    }

    serverAcceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true), errorCode);
    if(errorCode)
    {
        BOOST_LOG_TRIVIAL(error) << errorCode.message();
    }

    serverAcceptor.bind(endpoint, errorCode);
    if(errorCode)
    {
        BOOST_LOG_TRIVIAL(error) << errorCode.message();
    }

    BOOST_LOG_TRIVIAL(info) << "Starting accepting sockets on: " << hostname << ":" << port;
    serverAcceptor.listen(boost::asio::socket_base::max_listen_connections, errorCode);
    if(errorCode)
    {
        BOOST_LOG_TRIVIAL(error) << errorCode.message();
    }
    signals.add(SIGINT);
    signals.add(SIGTERM);
#if defined(SIGQUIT)
    signals.add(SIGQUIT);
#endif// defined(SIGQUIT


    router = std::make_shared<rest::Router>();
    router->addRoute<rest::Registration>("/registration");

    doAsyncStop();
    startAccept();
}

void server::TCPServer::startAccept()
{
    serverAcceptor.async_accept([this](boost::system::error_code errorCode, boost::asio::ip::tcp::socket socket) {
        if (errorCode)
        {
            std::cout << "Error: " << errorCode.message() << "\n";
        }
        else
        {
            std::cout << "Connected user\n";
            auto connection = std::make_shared<TCPConnection>(std::move(socket), router,*strand);
            connection->start();
            connection->setID(numberOfConnections);

            numberOfConnections++;
            connections[numberOfConnections] = connection;
        }
        startAccept();
    });
}

void server::TCPServer::run()
{
    int usedThreads = 0;
    if(config::ConfigManager::getConfig<config::ServerConfig>()->getUsedThreads() == -1)
    {
        usedThreads = std::thread::hardware_concurrency();
    }
    else
    {
        usedThreads = config::ConfigManager::getConfig<config::ServerConfig>()->getUsedThreads();
    }

    BOOST_LOG_TRIVIAL(info) << "Number of available threads: " << usedThreads;

    for (int i = 0; i < usedThreads; i++)
    {
        BOOST_LOG_TRIVIAL(info) << "Creating thread: " << i;
        mainThreadPool.create_thread([this]() {
            ioContext.run();
        });
    }
    mainThreadPool.join_all();
}

void server::TCPServer::doAsyncStop()
{
    signals.async_wait([this](const boost::system::error_code &error, int signalNumber) {
        BOOST_LOG_TRIVIAL(info) << "Closing server with signal : " << signalNumber << " Message : " << error.message();
        ioContext.stop();
    });
}
