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
#include "../http/Router.hpp"

namespace server
{
    class TCPConnection;
    class TCPServer
    {
    public:
        explicit TCPServer();
        void run();
        template<class T, typename ...Args>
        void addRoute(const std::string_view &route, Args &&...args)
        {
            router->addRoute<T,Args...>(route, std::forward<Args...>(args...));
        }
    private:
        void doAsyncStop();
        void startAccept();
        boost::asio::io_context ioContext;
        boost::asio::ip::tcp::acceptor serverAcceptor;
        boost::thread_group mainThreadPool;
        boost::asio::signal_set signals;
        boost::asio::executor_work_guard<decltype(ioContext.get_executor())> work{ioContext.get_executor()};
        std::shared_ptr<rest::Router> router;
    };

}// namespace server