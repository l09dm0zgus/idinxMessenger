//
// Created by cx9ps3 on 06.02.2024.
//
#pragma once

#include <boost/asio.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/system.hpp>
#include <boost/thread.hpp>
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
        void doAsyncStop();
        void startAccept();
        boost::asio::io_context ioContext;
        boost::asio::ip::tcp::acceptor serverAcceptor;
        boost::thread_group mainThreadPool;
        boost::asio::signal_set signals;
        boost::asio::executor_work_guard<decltype(ioContext.get_executor())> work{ioContext.get_executor()};
    };

}// namespace server