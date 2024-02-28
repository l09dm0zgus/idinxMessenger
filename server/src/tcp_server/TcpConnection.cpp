//
// Created by cx9ps3 on 06.02.2024.
//

#include "TcpConnection.hpp"
#include <iostream>
#include <boost/beast.hpp>
#include "../utils/Logger.hpp"

void server::TCPConnection::start()
{
    handleRead();
}

void server::TCPConnection::handleRead()
{
    request = {};
    auto self(shared_from_this());
    stream.expires_after(std::chrono::seconds(30));
    boost::beast::http::async_read(stream, buffer,request, [this, self](const boost::system::error_code &errorCode, std::size_t bytesTransferred) {
        if (errorCode)
        {
            BOOST_LOG_TRIVIAL(error) << errorCode.message();
        }
        if(errorCode == boost::beast::http::error::end_of_stream)
        {
            return close();
        }
        else
        {
            handleWrite(errorCode, bytesTransferred, router->handleRequest(request));
        }
    });
}

void server::TCPConnection::handleWrite(const boost::system::error_code &error, std::size_t bytesTransferred, const rest::Response &response)
{
    auto self(shared_from_this());
    boost::beast::http::async_write(stream, response, [this, self](const boost::system::error_code &errorCode, std::size_t bytesTransferred) {
        if(errorCode)
        {
            BOOST_LOG_TRIVIAL(error) << errorCode.message();
        }
        else
        {
            handleRead();
        }
    });
}

server::TCPConnection::TCPConnection(boost::asio::ip::tcp::socket &&socket, const std::shared_ptr<rest::Router> &newRouter) : router(newRouter), stream(std::move(socket))
{
}

void server::TCPConnection::close()
{
    boost::beast::error_code ec;
    stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
}
