//
// Created by cx9ps3 on 06.02.2024.
//
#include "TcpServer.hpp"
#include "TcpConnection.hpp"
#include <iostream>

server::TCPServer::TCPServer() : serverAcceptor(ioContext, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 1488))
{
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

void server::TCPServer::hanldeConnection(const std::shared_ptr<TCPConnection> &connection, const boost::system::error_code &errorCode)
{
    if (errorCode)
    {
        std::cout << "Error: " << errorCode.message() << "\n";
    }
    else
    {
        connection->start();
    }

    startAccept();
}

void server::TCPServer::run()
{
    ioContext.run();
}
