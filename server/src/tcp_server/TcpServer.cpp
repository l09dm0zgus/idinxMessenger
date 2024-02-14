//
// Created by cx9ps3 on 06.02.2024.
//
#include "TcpServer.hpp"
#include "../utils/Logger.hpp"
#include "TcpConnection.hpp"
#include <iostream>

server::TCPServer::TCPServer() : serverAcceptor(ioContext, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 1488)), signals(ioContext)
{
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
    for (int i = 0; i < 4; i++)
    {
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
