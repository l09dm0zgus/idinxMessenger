//
// Created by cx9ps3 on 03.06.2024.
//
#include "IRoute.hpp"

std::string rest::IRoute::createResponseBody(const std::string_view &what, StatusCodes code)
{
    json::object obj;
    std::stringstream ss;
    obj["response"] = {
            {"code", static_cast<int>(code)},
            {"what", what}};
    ss << obj;
    return ss.str();
}

std::shared_ptr<rest::Response> rest::IRoute::createResponse(boost::beast::http::status status, const std::string_view &what, StatusCodes statusCodes, const Request &request)
{
    auto response = std::make_shared<Response>(status, request.version());
    response->set(boost::beast::http::field::server, "idinxServer/1488");
    response->set(boost::beast::http::field::content_type, "application/json");
    response->keep_alive(request.keep_alive());
    response->body() = createResponseBody(what, statusCodes);
    response->prepare_payload();
    return response;
}
