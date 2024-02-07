//
// Created by cx9ps3 on 06.02.2024.
//

#include "TcpConnection.hpp"
#include <iostream>

void server::TCPConnection::start()
{
    handleRead();
}

void server::TCPConnection::handleRead()
{
    auto self(shared_from_this());
    clientSocket.async_read_some(boost::asio::buffer(buffer),[this, self](const boost::system::error_code& errorCode, std::size_t bytesTransferred) {
        if(errorCode)
        {
            std::cout << "Error: " << errorCode.message() << "\n";
        }
        else
        {
            for(auto c : buffer)
            {
                std::cout << c;
            }
            std::cout << "\n";

            handleWrite(errorCode, bytesTransferred);
        }
    });
}

void server::TCPConnection::handleWrite(const boost::system::error_code &error, std::size_t bytesTransferred)
{
    auto self(shared_from_this());
    clientSocket.async_write_some(boost::asio::buffer(buffer),[this, self](const boost::system::error_code& errorCode, std::size_t bytesTransferred) {
        if(errorCode)
        {
            std::cout << "Error: " << errorCode.message() << "\n";
        }
        else
        {
            handleRead();
        }
    });
}

server::TCPConnection::TCPConnection(boost::asio::ip::tcp::socket socket) : clientSocket(std::move(socket))
{

}

