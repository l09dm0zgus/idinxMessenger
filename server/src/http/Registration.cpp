//
// Created by cx9ps3 on 03.06.2024.
//

#include "Registration.hpp"
#include "../database/Connection.hpp"
#include "../tcp_server/TcpConnection.hpp"
#include "../utils/Hashing.hpp"
#include "../utils/Logger.hpp"

std::shared_ptr<rest::Response> rest::Registration::handleRequest(const server::Connection &clientConnection, const rest::Request &request)
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

    if (checkIfEmailExistsInDatabase(userData.email, response, request))
    {
        return IRoute::createResponse(boost::beast::http::status::ok, "User with this login exist!", StatusCodes::USER_WITH_EMAIL_EXIST, request);
    }

    if (!response->body().empty())
    {
        return response;
    }

    if (checkIfLoginExistsInDatabase(userData.login, response, request))
    {
        return IRoute::createResponse(boost::beast::http::status::ok, "User with this email exist!", StatusCodes::USER_WITH_LOGIN_EXIST, request);
    }

    if (!response->body().empty())
    {
        return response;
    }

    writeRegistrationDataToDatabase(userData, response, request);
    return response;
}

bool rest::Registration::decryptBody(const server::Connection &clientConnection, const rest::Request &request)
{
    return clientConnection->decrypt(request.body(), decryptedBody);
}
rest::Registration::UserRegistrationData rest::Registration::parseBody(const server::Connection &clientConnection, std::shared_ptr<Response> &response, const Request &request)
{
    UserRegistrationData userRegistrationData;
    try
    {
        auto values = json::parse(decryptedBody);
        userRegistrationData.email = values.at("user_data").at("email").as_string();
        userRegistrationData.login = values.at("user_data").at("login").as_string();
        userRegistrationData.password = values.at("user_data").at("password").as_string();
        userRegistrationData.id = clientConnection->getID();
    }
    catch (std::exception &ex)
    {
        userRegistrationData.id = -1;
        response = IRoute::createResponse(boost::beast::http::status::bad_request, ex.what(), StatusCodes::FAILED_TO_PARSE_BODY, request);
    }
    return userRegistrationData;
}

bool rest::Registration::checkIfLoginExistsInDatabase(const std::string &login, std::shared_ptr<Response> &response, const Request &request)
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

bool rest::Registration::checkIfEmailExistsInDatabase(const std::string &email, std::shared_ptr<Response> &response, const Request &request)
{
    pqxx::result result;
    try
    {
        auto connection = db::Connection::connect();
        auto work = connection->getWork();
        result = work->exec_params("SELECT * FROM users WHERE email=$1", email);
    }
    catch (const std::exception &ex)
    {
        BOOST_LOG_TRIVIAL(error) << ex.what();
        response = IRoute::createResponse(boost::beast::http::status::bad_request, ex.what(), StatusCodes::DATABASE_NOT_RESPONDING, request);
    }

    return !result.empty();
}

void rest::Registration::writeRegistrationDataToDatabase(const rest::Registration::UserRegistrationData &userRegistrationData, std::shared_ptr<Response> &response, const rest::Request &request)
{
    try
    {
        auto connection = db::Connection::connect();
        auto work = connection->getWork();
        work->exec_params("INSERT INTO users (user_id, email, login, password) VALUES ($1, $2, $3, $4); ", userRegistrationData.id, userRegistrationData.email, userRegistrationData.login, utils::hashPassword(userRegistrationData.password));
        work->commit();
    }
    catch (const std::exception &ex)
    {
        BOOST_LOG_TRIVIAL(error) << ex.what();
        response = IRoute::createResponse(boost::beast::http::status::bad_request, ex.what(), StatusCodes::DATABASE_NOT_RESPONDING, request);
    }
    if (response->body().empty())
    {
        response = IRoute::createResponse(boost::beast::http::status::ok, "Registration successful.", StatusCodes::OK, request);
    }
}
