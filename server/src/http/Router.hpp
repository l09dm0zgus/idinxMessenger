//
// Created by cx9ps3 on 28.02.2024.
//

#pragma once
#include "IRoute.hpp"
#include <memory>
#include <string_view>
#include <unordered_map>

namespace rest
{

    class Router
    {
    private:
        std::unordered_map<std::string_view, std::shared_ptr<IRoute>> routes;

    public:
        std::shared_ptr<Response> handleRequest(const server::Connection &clientConnection, const Request &request);

        template<class T, typename ...Args>
        void addRoute(const std::string_view &route, Args &&...args)
        {
            routes[route] = std::make_shared<T>(std::forward<Args>(args)...);
        }
    };
}// namespace rest
