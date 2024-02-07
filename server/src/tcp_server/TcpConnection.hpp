//
// Created by cx9ps3 on 06.02.2024.
//

#pragma once
#include <array>
#include <boost/asio.hpp>
#include <boost/system.hpp>

namespace server
{
    class TCPConnection : public std::enable_shared_from_this<TCPConnection>
    {
    public:
        explicit TCPConnection(boost::asio::ip::tcp::socket socket);
        void start();
    private:

        static constexpr int BUFFER_SIZE = 1024;
        std::array<char, BUFFER_SIZE> buffer {0};
        boost::asio::ip::tcp::socket clientSocket;

        void handleWrite(const boost::system::error_code& error, std::size_t bytesTransferred);
        void handleRead();

    };
}

