//
// Created by cx9ps3 on 6/15/24.
//

#include "Login.hpp"
#include "../database/Connection.hpp"
#include "../tcp_server/TcpConnection.hpp"
#include "../utils/Hashing.hpp"
#include "../utils/Logger.hpp"

rest::Login::Login(std::unordered_map<long long int, std::shared_ptr<server::TCPConnection>> &newConnections) : connections(newConnections)
{
}

std::shared_ptr<rest::Response> rest::Login::handleRequest(const server::Connection &clientConnection, const rest::Request &request)
{
    if (!decryptBody(clientConnection, request))
    {
        return IRoute::createResponse(boost::beast::http::status::bad_request, "Failed to decrypt request body!", StatusCodes::FAILED_TO_DECRYPT_BODY, request);
    }

    std::shared_ptr<Response> response = std::make_shared<Response>();
    auto userData = parseBody(clientConnection, response, request);
    if (!response->body().empty())
    {
        return response;
    }

    auto isLoginExist = checkIfLoginExistsInDatabase(userData.first, response, request);

    if (!response->body().empty())
    {
        return response;
    }

    if (!isLoginExist)
    {
        return IRoute::createResponse(boost::beast::http::status::ok, "Wrong user login!", StatusCodes::WRONG_USER_LOGIN, request);
    }

    auto isPasswordExist = checkIfLoginExistsInDatabase(userData.first, response, request);

    if (!response->body().empty())
    {
        return response;
    }

    if (!isPasswordExist)
    {
        return IRoute::createResponse(boost::beast::http::status::ok, "Wrong user password!", StatusCodes::WRONG_USER_PASSWORD, request);
    }

    if (isPasswordExist && isLoginExist)
    {
        response = IRoute::createResponse(boost::beast::http::status::ok, "Login successful.", StatusCodes::OK, request);
        auto it = connections.find(clientConnection->getID());
        connections[userID] = clientConnection;
        connections.erase(it);
    }
    return response;
}

bool rest::Login::decryptBody(const server::Connection &clientConnection, const rest::Request &request)
{
    return clientConnection->decrypt(request.body(), decryptedBody);
    ;
}

std::pair<std::string, std::string> rest::Login::parseBody(const server::Connection &clientConnection, std::shared_ptr<Response> &response, const rest::Request &request)
{
    try
    {
        auto values = json::parse(decryptedBody);
        decryptedBody = "";
        auto login = values.at("user_data").at("login").as_string().c_str();
        auto password = values.at("user_data").at("password").as_string().c_str();
        return {login, password};
    }
    catch (std::exception &ex)
    {
        BOOST_LOG_TRIVIAL(error) << ex.what();
        response = IRoute::createResponse(boost::beast::http::status::bad_request, ex.what(), StatusCodes::FAILED_TO_PARSE_BODY, request);
        return {"", ""};
    }
}

bool rest::Login::checkIfLoginExistsInDatabase(const std::string &login, std::shared_ptr<Response> &response, const rest::Request &request)
{
    pqxx::result result;
    try
    {
        auto connection = db::Connection::connect();
        auto work = connection->getWork();
        result = work->exec_params("SELECT * FROM users WHERE login=$1", login);
    }
    catch (const std::exception &ex)
    {
        BOOST_LOG_TRIVIAL(error) << ex.what();
        response = IRoute::createResponse(boost::beast::http::status::bad_request, ex.what(), StatusCodes::DATABASE_NOT_RESPONDING, request);
    }

    return !result.empty();
}

bool rest::Login::checkIfPasswordExistsInDatabase(const std::string &login, const std::string &password, std::shared_ptr<Response> &response, const Request &request)
{
    pqxx::result result;
    try
    {
        auto connection = db::Connection::connect();
        auto work = connection->getWork();
        result = work->exec_params("SELECT password,user_id FROM users WHERE login=$1", login);
    }
    catch (const std::exception &ex)
    {
        BOOST_LOG_TRIVIAL(error) << ex.what();
        response = IRoute::createResponse(boost::beast::http::status::bad_request, ex.what(), StatusCodes::DATABASE_NOT_RESPONDING, request);
        return false;
    }

    userID = result[0]["user_id"].as<long long>();

    for (auto const &row: result)
    {
        for (auto const &field: row)
        {
            return utils::checkIfPasswordsEqual(password, field.as<std::string>());
        }
    }
    return false;
}
