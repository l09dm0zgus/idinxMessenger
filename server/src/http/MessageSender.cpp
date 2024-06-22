//
// Created by cx9ps3 on 6/17/24.
//

#include "MessageSender.hpp"
#include "../utils/Logger.hpp"
#include "../tcp_server/TcpConnection.hpp"

rest::MessageSender::MessageSender(std::unordered_map<long long int, std::shared_ptr<server::TCPConnection>> &newConnections) : KeyRequester(newConnections)
{

}

bool rest::MessageSender::parseBody(const std::string_view &body, std::shared_ptr<Response> &response, const rest::Request &request)
{
    try
    {
        auto values = json::parse(body);

        setSenderID(values.at("user_message").at("sender_id").as_int64());
        setReceiverLogin(values.at("user_message").at("receiver_login").as_string().c_str());
        setPublicKey(values.at("user_message").at("public_key").as_string().c_str());

        encryptedAESKey = values.at("user_message").at("key").as_string().c_str();
        encryptedMessage = values.at("user_message").at("message").as_string().c_str();

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
std::shared_ptr<rest::Response> rest::MessageSender::handleRequest(const server::Connection &clientConnection, const rest::Request &request)
{
    auto response = std::make_shared<Response>();
    if(!parseBody(request.body(),response,request))
    {
        return response;
    }

    if(!checkSenderIDInDatabase(response,request))
    {
        return response;
    }

    if(!checkReceiverLoginInDatabase(response,request))
    {
        return response;
    }

    if(checkIfSameUser(response,request))
    {
        return response;
    }

    if(getConnection().count(getReceiverID()))
    {
        getConnection()[getReceiverID()]->send(createMessage());
    }
    else
    {
        if(!writeMessageToDatabaseIfReceiverOffline(MessageType::STANDARD_MESSAGE,response,request))
        {
            return response;
        }
    }


    response = IRoute::createResponse(boost::beast::http::status::ok, "Message send successful!", StatusCodes::OK, getSenderID(), request);
    return response;
}

std::string rest::MessageSender::createMessage()
{
    boost::json::object obj;
    obj["user_message"] =
            {       {"type",static_cast<int>(MessageType::STANDARD_MESSAGE)},
                    {"sender" , getSenderLogin()},
                    {"receiver", getReceiverLogin()},
                    {"sender_public_key", getPublicKey()},
                    {"encrypted_key",encryptedAESKey},
                    {"message",encryptedMessage}
            };
    std::stringstream ss;
    ss << obj;
    return ss.str();
}
