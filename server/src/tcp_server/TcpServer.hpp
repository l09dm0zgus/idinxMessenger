//
// Created by cx9ps3 on 06.02.2024.
//
#pragma once

#include "../http/Router.hpp"
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
        template<class T, typename... Args>
        void addRoute(const std::string_view &route, Args &&...args)
        {
            router->addRoute<T, Args...>(route, std::forward<Args...>(args...));
        }

    private:
        void doAsyncStop();
        void startAccept();
        std::unordered_map<long long, std::shared_ptr<TCPConnection>> connections;
        long long numberOfConnections{0};
        boost::asio::io_context ioContext;
        boost::asio::ip::tcp::acceptor serverAcceptor;
        boost::thread_group mainThreadPool;
        std::shared_ptr<boost::asio::io_context::strand> strand;
        boost::asio::signal_set signals;
        boost::asio::executor_work_guard<decltype(ioContext.get_executor())> work{ioContext.get_executor()};
        std::shared_ptr<rest::Router> router;
    };

}// namespace server