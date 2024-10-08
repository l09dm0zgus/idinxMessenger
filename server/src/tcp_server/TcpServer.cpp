//
// Created by cx9ps3 on 06.02.2024.
//
#include "TcpServer.hpp"
#include "../config/ConfigManager.hpp"
#include "../config/ServerConfig.hpp"
#include "../http/Login.hpp"
#include "../http/Registration.hpp"
#include "../http/KeyRequester.hpp"
#include "../http/MessageSender.hpp"
#include "../http/MessageReceiver.hpp"
#include "TcpConnection.hpp"
#include <iostream>


server::TCPServer::TCPServer() : serverAcceptor(ioContext), signals(ioContext)
{
    boost::asio::ip::tcp::resolver resolver(ioContext);
    strand = std::make_shared<boost::asio::io_context::strand>(ioContext);
    auto hostname = config::ConfigManager::getConfig<config::ServerConfig>()->getHostname();
    auto port = config::ConfigManager::getConfig<config::ServerConfig>()->getPort();

    boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(hostname, std::to_string(port)).begin();

    boost::beast::error_code errorCode;

    serverAcceptor.open(endpoint.protocol(), errorCode);
    if (errorCode)
    {
        BOOST_LOG_TRIVIAL(error) << errorCode.message();
    }

    serverAcceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true), errorCode);
    if (errorCode)
    {
        BOOST_LOG_TRIVIAL(error) << errorCode.message();
    }

    serverAcceptor.bind(endpoint, errorCode);
    if (errorCode)
    {
        BOOST_LOG_TRIVIAL(error) << errorCode.message();
    }

    BOOST_LOG_TRIVIAL(info) << "Starting accepting sockets on: " << hostname << ":" << port;
    serverAcceptor.listen(boost::asio::socket_base::max_listen_connections, errorCode);
    if (errorCode)
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
    router->addRoute<rest::Login>("/login", connections);
    router->addRoute<rest::KeyRequester>("/keyExchangeRequest",connections);
    router->addRoute<rest::MessageSender>("/sendMessage",connections);
    router->addRoute<rest::MessageReceiver>("/availableMessages");

    doAsyncStop();
    startAccept();
}

void server::TCPServer::startAccept()
{
    serverAcceptor.async_accept([this](boost::system::error_code errorCode, boost::asio::ip::tcp::socket socket) {
        if (errorCode)
        {
            BOOST_LOG_TRIVIAL(error) << "Error: " << errorCode.message() << "\n";
        }
        else
        {
            auto connection = std::make_shared<TCPConnection>(std::move(socket), router, *strand);
            connection->start();
            numberOfConnections++;

            CryptoPP::AutoSeededRandomPool rng;
            CryptoPP::Integer id(rng, 0, std::numeric_limits<long>::max());

            connection->setID(id.ConvertToLong());
            connections[id.ConvertToLong()] = connection;
            BOOST_LOG_TRIVIAL(info) << "Connected client with id: " << id.ConvertToLong() << "\n";
        }
        startAccept();
    });
}

void server::TCPServer::run()
{
    int usedThreads = 0;
    if (config::ConfigManager::getConfig<config::ServerConfig>()->getUsedThreads() == -1)
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
