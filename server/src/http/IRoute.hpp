//
// Created by cx9ps3 on 28.02.2024.
//

#pragma once
#include <boost/beast.hpp>

namespace rest
{
    using Request = boost::beast::http::request<boost::beast::http::string_body>;
    using Response = boost::beast::http::response<boost::beast::http::string_body>;
    class IRoute
    {
    public:
        virtual Response handleRequest(const Request &request) = 0;
    };
}// namespace rest