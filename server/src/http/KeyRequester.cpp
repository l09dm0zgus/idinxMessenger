//
// Created by cx9ps3 on 6/17/24.
//

#include "KeyRequester.hpp"
#include "../database/Connection.hpp"
#include "../tcp_server/TcpConnection.hpp"
#include "../utils/Logger.hpp"

rest::KeyRequester::KeyRequester(std::unordered_map<long long int, std::shared_ptr<server::TCPConnection>> &newConnections) : connections(newConnections)
{
}

std::shared_ptr<rest::Response> rest::KeyRequester::handleRequest(const server::Connection &clientConnection, const rest::Request &request)
{
    auto response = std::make_shared<Response>();
    if (!parseBody(request.body(), response, request))
    {
        return response;
    }

    if (!checkSenderIDInDatabase(response, request))
    {
        return response;
    }

    if (!checkReceiverLoginInDatabase(response, request))
    {
        return response;
    }

    if (checkIfSameUser(response, request))
    {
        return response;
    }

    if (connections.count(receiverID))
    {
        connections[receiverID]->asyncSend(createKeyExchangeMessage());
    }
    else
    {
        if (!writeMessageToDatabaseIfReceiverOffline(MessageType::EXCHANGE_KEYS_MESSAGE, response, request))
        {
            return response;
        }
    }


    response = IRoute::createResponse(boost::beast::http::status::ok, "Request for key exchange send successful!", StatusCodes::OK, senderID, request);
    return response;
}

bool rest::KeyRequester::parseBody(const std::string_view &body, std::shared_ptr<Response> &response, const Request &request)
{
    try
    {
        auto values = json::parse(body);
        senderID = values.at("request_key").at("sender_id").as_int64();
        receiverLogin = values.at("request_key").at("receiver_login").as_string().c_str();
        publicKey = values.at("request_key").at("public_key").as_string().c_str();
        return true;
    }
    catch (std::exception &ex)
    {
        BOOST_LOG_TRIVIAL(error) << ex.what();
        response = IRoute::createResponse(boost::beast::http::status::bad_request, ex.what(), StatusCodes::FAILED_TO_PARSE_BODY, -1, request);
        return false;
    }

    return false;
}

bool rest::KeyRequester::checkSenderIDInDatabase(std::shared_ptr<Response> &response, const Request &request)
{
    pqxx::result result;
    try
    {
        auto connection = db::Connection::connect();
        auto work = connection->getWork();
        result = work->exec_params("SELECT * FROM users WHERE user_id=$1", senderID);
    }
    catch (const std::exception &ex)
    {
        BOOST_LOG_TRIVIAL(error) << ex.what();
        response = IRoute::createResponse(boost::beast::http::status::bad_request, ex.what(), StatusCodes::DATABASE_NOT_RESPONDING, -1, request);
        return false;
    }

    if (result.empty())
    {
        response = IRoute::createResponse(boost::beast::http::status::bad_request, "wrong user id!", StatusCodes::WRONG_USER_ID, -1, request);
        return false;
    }

    senderLogin = result[0]["login"].as<std::string>();
    return true;
}

bool rest::KeyRequester::checkReceiverLoginInDatabase(std::shared_ptr<Response> &response, const rest::Request &request)
{
    pqxx::result result;
    try
    {
        auto connection = db::Connection::connect();
        auto work = connection->getWork();
        result = work->exec_params("SELECT * FROM users WHERE login=$1", receiverLogin);
    }
    catch (const std::exception &ex)
    {
        BOOST_LOG_TRIVIAL(error) << ex.what();
        response = IRoute::createResponse(boost::beast::http::status::bad_request, ex.what(), StatusCodes::DATABASE_NOT_RESPONDING, -1, request);
        return false;
    }

    if (result.empty())
    {
        response = IRoute::createResponse(boost::beast::http::status::bad_request, "Wrong user id!", StatusCodes::WRONG_USER_ID, -1, request);
        return false;
    }

    receiverID = result[0]["user_id"].as<long long>();
    return true;
}
std::string rest::KeyRequester::createKeyExchangeMessage()
{
    boost::json::object obj;
    obj["request_key"] =
            {
                    {"type",static_cast<int>(MessageType::EXCHANGE_KEYS_MESSAGE)},
                    {"sender", senderLogin},
                    {"receiver", receiverLogin},
                    {"sender_public_key", publicKey}};
    std::stringstream ss;
    ss << obj;
    return ss.str();
}

void rest::KeyRequester::setPublicKey(const std::string &newPublicKey)
{
    publicKey = newPublicKey;
}

std::unordered_map<long long, std::shared_ptr<server::TCPConnection>> &rest::KeyRequester::getConnection() const noexcept
{
    return connections;
}

void rest::KeyRequester::setSenderID(long long newSenderID)
{
    senderID = newSenderID;
}

void rest::KeyRequester::setReceiverLogin(const std::string newReceiverLogin)
{
    receiverLogin = newReceiverLogin;
}

std::string rest::KeyRequester::getPublicKey() const noexcept
{
    return publicKey;
}

std::string rest::KeyRequester::getReceiverLogin() const noexcept
{
    return receiverLogin;
}

std::string rest::KeyRequester::getSenderLogin() const noexcept
{
    return senderLogin;
}

long long rest::KeyRequester::getReceiverID() const noexcept
{
    return receiverID;
}

bool rest::KeyRequester::writeMessageToDatabaseIfReceiverOffline(rest::MessageType messageType, std::shared_ptr<Response> &response, const rest::Request &request)
{
    try
    {
        auto connection = db::Connection::connect();
        auto work = connection->getWork();
        work->exec_params("INSERT INTO messages (message_type, sender, receiver, message) VALUES ($1, $2, $3, $4); ", static_cast<int>(messageType), senderLogin, receiverLogin, createKeyExchangeMessage());
        work->commit();
    }
    catch (const std::exception &ex)
    {
        BOOST_LOG_TRIVIAL(error) << ex.what();
        response = IRoute::createResponse(boost::beast::http::status::bad_request, ex.what(), StatusCodes::DATABASE_NOT_RESPONDING, -1, request);
        return true;
    }
    return true;
}

long long rest::KeyRequester::getSenderID() const noexcept
{
    return senderID;
}

bool rest::KeyRequester::checkIfSameUser(std::shared_ptr<Response> &response, const rest::Request &request)
{
    if (senderLogin == receiverLogin || senderID == receiverID)
    {
        response = IRoute::createResponse(boost::beast::http::status::ok, "You cannot messaging with yourself!", StatusCodes::MESSAGING_WITH_YOURSELF, -1, request);
        return true;
    }
    else
    {
        return false;
    }
}
