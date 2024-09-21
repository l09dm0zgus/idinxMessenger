//
// Created by cx9ps3 on 6/17/24.
//

#include "MessageReceiver.hpp"
#include "../database/Connection.hpp"
#include "../tcp_server/TcpConnection.hpp"
#include "../utils/Hashing.hpp"
#include "../utils/Logger.hpp"
#include <boost/url.hpp>

bool rest::MessageReceiver::parseQuery(std::shared_ptr<Response> &response, const rest::Request &request)
{
    try
    {
        auto result = boost::urls::parse_origin_form(getURL());
        auto urlView = result.value();
        auto params = urlView.params();
        for(const auto& param: params)
        {
            if(param.key == "login")
            {
                login = param.value;
            }
            else if(param.key == "password")
            {
                encryptedPassword = param.value;
            }
            else
            {
                response = IRoute::createResponse(boost::beast::http::status::bad_request, "Incorrect url query!", StatusCodes::INCORRECT_URL_QUERY, -1, request);
                return false;
            }
        }
    }
    catch(boost::system::error_code &ex)
    {
        response = IRoute::createResponse(boost::beast::http::status::bad_request, ex.what(), StatusCodes::INCORRECT_URL_QUERY, -1, request);
        return false;
    }

    return true;
}

bool rest::MessageReceiver::decryptUserPassword(const server::Connection &clientConnection, const rest::Request &request)
{
    return clientConnection->decrypt(encryptedPassword,decryptedPassword);
}

bool rest::MessageReceiver::checkIfLoginExistsInDatabase(std::shared_ptr<Response> &response, const rest::Request &request)
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
        response = IRoute::createResponse(boost::beast::http::status::bad_request, ex.what(), StatusCodes::DATABASE_NOT_RESPONDING, -1, request);
        return false;
    }

    return !result.empty();
}


bool rest::MessageReceiver::checkIfPasswordExistsInDatabase(std::shared_ptr<Response> &response, const rest::Request &request)
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
        response = IRoute::createResponse(boost::beast::http::status::bad_request, ex.what(), StatusCodes::DATABASE_NOT_RESPONDING, -1, request);
        return false;
    }

    userID = result[0]["user_id"].as<long long>();

    for (auto const &row: result)
    {
        for (auto const &field: row)
        {
            return utils::checkIfPasswordsEqual(decryptedPassword, field.as<std::string>());
        }
    }

    return false;
}

bool rest::MessageReceiver::getAllMessagesForUser(std::shared_ptr<Response> &response, const rest::Request &request)
{
    boost::json::array jsonArray;
    try
    {
        auto connection = db::Connection::connect();
        auto work = connection->getWork();
        auto result = work->exec_params("SELECT * FROM messages WHERE receiver=$1",login);
        for(const auto& row : result)
        {
            jsonArray.push_back(row["message"].as<std::string>().c_str());
        }

        work->exec_params("DELETE FROM messages WHERE receiver=$1",login);
        work->commit();
    }
    catch (const std::exception &ex)
    {
        BOOST_LOG_TRIVIAL(error) << ex.what();
        response = IRoute::createResponse(boost::beast::http::status::bad_request, ex.what(), StatusCodes::DATABASE_NOT_RESPONDING, -1, request);
        return false;
    }

    std::stringstream  ss;
    ss << jsonArray;
    allUserMessages = ss.str();

    return true;
}

std::shared_ptr<rest::Response> rest::MessageReceiver::handleRequest(const server::Connection &clientConnection, const rest::Request &request)
{
    auto response = std::make_shared<Response>();
    if(!parseQuery(response,request))
    {
        return response;
    }

    if(!decryptUserPassword(clientConnection,request))
    {
        return IRoute::createResponse(boost::beast::http::status::bad_request, "Failed to decrypt query!", StatusCodes::FAILED_TO_DECRYPT_BODY, -1, request);
    }

    if(!checkIfLoginExistsInDatabase(response,request))
    {
        return response;
    }

    if(!checkIfPasswordExistsInDatabase(response,request))
    {
        return response;
    }

    if(getAllMessagesForUser(response,request))
    {
        return response;
    }

    return createResponseWithMessages(request);
}

std::shared_ptr<rest::Response> rest::MessageReceiver::createResponseWithMessages(const rest::Request &request)
{
    auto response = std::make_shared<Response>(boost::beast::http::status::ok, request.version());
    response->set(boost::beast::http::field::server, "idinxServer/1488");
    response->set(boost::beast::http::field::content_type, "application/json");
    response->keep_alive(request.keep_alive());
    response->body() = allUserMessages;
    response->prepare_payload();
    return response;
}
