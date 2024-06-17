//
// Created by cx9ps3 on 28.02.2024.
//

#pragma once
#include <boost/beast.hpp>
#include <boost/json.hpp>
#include <sstream>

namespace server
{
    class TCPConnection;
    using Connection = std::shared_ptr<TCPConnection>;
}// namespace server

namespace rest
{
    using Request = boost::beast::http::request<boost::beast::http::string_body>;
    using Response = boost::beast::http::response<boost::beast::http::string_body>;
    namespace json = boost::json;

    enum class StatusCodes
    {
        FAILED_TO_DECRYPT_BODY = -100,
        FAILED_TO_PARSE_BODY,
        USER_WITH_LOGIN_EXIST,
        USER_WITH_EMAIL_EXIST,
        WRONG_USER_LOGIN,
        WRONG_USER_PASSWORD,
        DATABASE_NOT_RESPONDING,
        OK = 1,
    };

    class IRoute
    {
    private:
        static std::string createResponseBody(const std::string_view &what, StatusCodes code, long long sessionID);

    protected:
        static std::shared_ptr<Response> createResponse(boost::beast::http::status status, const std::string_view &what, StatusCodes statusCodes, long long sessionID, const rest::Request &request);

    public:
        virtual ~IRoute() = default;
        virtual std::shared_ptr<Response> handleRequest(const server::Connection &clientConnection, const Request &request) = 0;
    };
}// namespace rest