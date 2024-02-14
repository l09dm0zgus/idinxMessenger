//
// Created by cx9ps3 on 06.02.2024.
//
#include "TcpServer.hpp"
#include "../utils/Logger.hpp"
#include "../config/ConfigManager.hpp"
#include "../config/ServerConfig.hpp"
#include "TcpConnection.hpp"
#include <iostream>

server::TCPServer::TCPServer() : serverAcceptor(ioContext), signals(ioContext)
{
    boost::asio::ip::tcp::resolver resolver(ioContext);

    auto hostname = config::ConfigManager::getConfig<config::ServerConfig>()->getHostname();
    auto port = config::ConfigManager::getConfig<config::ServerConfig>()->getPort();

    boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(hostname, std::to_string(port)).begin();
    serverAcceptor.open(endpoint.protocol());
    serverAcceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    serverAcceptor.bind(endpoint);
    BOOST_LOG_TRIVIAL(info) << "Starting accepting sockets on: " << hostname << ":" << port;
    serverAcceptor.listen();

    signals.add(SIGINT);
    signals.add(SIGTERM);
#if defined(SIGQUIT)
    signals.add(SIGQUIT);
#endif// defined(SIGQUIT

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
            std::make_shared<TCPConnection>(std::move(socket))->start();
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

    BOOST_LOG_TRIVIAL(info) << "Number of aviable threads: " << usedThreads;

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
