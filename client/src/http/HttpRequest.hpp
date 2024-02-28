//
// Created by cx9ps3 on 27.02.2024.
//

#pragma once
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <iostream>
#undef DELETE

namespace rest
{
    enum class Method
    {
        GET,
        POST,
        PATCH,
        DELETE
    };

    template<Method T>
    class HttpRequest;

    template<class Body>
    class BaseHttpRequest
    {
    private:
        std::shared_ptr<boost::beast::http::request<Body>> request;

    public:
        BaseHttpRequest(const std::string &route, const std::string &host);
        std::shared_ptr<boost::beast::http::request<Body>> getBeastRequestObject();
    };

    template<class Body>
    std::shared_ptr<boost::beast::http::request<Body>> BaseHttpRequest<Body>::getBeastRequestObject()
    {
        return request;
    }

    template<class Body>
    BaseHttpRequest<Body>::BaseHttpRequest(const std::string &route, const std::string &host)
    {
        request = std::make_shared<boost::beast::http::request<Body>>();
        getBeastRequestObject()->version(11);
        getBeastRequestObject()->target(route);
        getBeastRequestObject()->set(boost::beast::http::field::host, host);
        getBeastRequestObject()->set(boost::beast::http::field::user_agent, "idinxClient/1488");
    }

    template<>
    class HttpRequest<Method::GET> : public BaseHttpRequest<boost::beast::http::empty_body>
    {
    public:
        explicit HttpRequest(const std::string &route, const std::string &host) : BaseHttpRequest<boost::beast::http::empty_body>(route, host)
        {
            getBeastRequestObject()->method(boost::beast::http::verb::get);
        }
    };

    template<>
    class HttpRequest<Method::POST> : public BaseHttpRequest<boost::beast::http::string_body>
    {
    public:
        explicit HttpRequest(const std::string &route, const std::string &host, const std::string &mimeType, const std::string &bodyData) : BaseHttpRequest<boost::beast::http::string_body>(route, host)
        {
            getBeastRequestObject()->method(boost::beast::http::verb::post);
            getBeastRequestObject()->set(boost::beast::http::field::content_type, mimeType);
            getBeastRequestObject()->body() = bodyData;
        }
    };

    template<>
    class HttpRequest<Method::PATCH> : public BaseHttpRequest<boost::beast::http::string_body>
    {
    public:
        explicit HttpRequest(const std::string &route, const std::string &host, const std::string &mimeType, const std::string &bodyData) : BaseHttpRequest<boost::beast::http::string_body>(route, host)
        {
            getBeastRequestObject()->method(boost::beast::http::verb::patch);
            getBeastRequestObject()->set(boost::beast::http::field::content_type, mimeType);
            getBeastRequestObject()->body() = bodyData;
        }
    };

    template<>
    class HttpRequest<Method::DELETE> : public BaseHttpRequest<boost::beast::http::empty_body>
    {
    public:
        explicit HttpRequest(const std::string &route, const std::string &host) : BaseHttpRequest<boost::beast::http::empty_body>(route, host)
        {
            getBeastRequestObject()->method(boost::beast::http::verb::delete_);
        }
    };
}// namespace rest
