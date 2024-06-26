//
// Created by cx9ps3 on 28.02.2024.
//

#include "Router.hpp"
#include <boost/beast/http.hpp>
#include <boost/url.hpp>
#include <iostream>


std::shared_ptr<rest::Response> rest::Router::handleRequest(const server::Connection &clientConnection, const rest::Request &request)
{
    auto const badRequest = [&request](std::string_view why) {
        auto response = std::make_shared<Response>(boost::beast::http::status::bad_request, request.version());
        response->set(boost::beast::http::field::server, "idinxServer/1488");
        response->set(boost::beast::http::field::content_type, "text/html");
        response->keep_alive(request.keep_alive());
        response->body() = std::string(why);
        response->prepare_payload();
        return response;
    };

    auto const notFound = [&request](std::string_view target) {
        auto response = std::make_shared<Response>(boost::beast::http::status::not_found, request.version());
        response->set(boost::beast::http::field::server, "idinxServer/1488");
        response->set(boost::beast::http::field::content_type, "text/html");
        response->keep_alive(request.keep_alive());
        response->body() = "The resource '" + std::string(target) + "' was not found.";
        response->prepare_payload();
        return response;
    };

    if (request.method() != boost::beast::http::verb::get && request.method() != boost::beast::http::verb::post && request.method() != boost::beast::http::verb::patch && request.method() != boost::beast::http::verb::delete_)
    {
        return badRequest("Unknown HTTP-method");
    }

    if (request.target().empty() || request.target()[0] != '/' || request.target().find("..") != std::string_view::npos)
    {
        return badRequest("Illegal request-target");
    }

    auto target = request.target();
    auto result = boost::urls::parse_origin_form(target);
    auto urlView = result.value();
    auto path = urlView.path();

    if (routes.count(path))
    {
        routes[target]->setURL(target);
        return routes[target]->handleRequest(clientConnection, request);
    }
    else
    {
        return notFound(target);
    }
}
