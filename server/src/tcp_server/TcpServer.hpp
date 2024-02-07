//
// Created by cx9ps3 on 06.02.2024.
//
#pragma once

#include <boost/asio.hpp>
#include <boost/system.hpp>
#include <memory>
#include <vector>

namespace server
{
    class TCPConnection;
    class TCPServer
    {
    public:
        explicit TCPServer();
        void run();
    private:
        std::vector<std::shared_ptr<TCPConnection>> clients;
        void startAccept();
        void hanldeConnection(const std::shared_ptr<TCPConnection> &connection, const boost::system::error_code &errorCode);
        boost::asio::io_context ioContext;
        boost::asio::ip::tcp::acceptor serverAcceptor;
    };

}