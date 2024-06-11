//
// Created by cx9ps3 on 03.06.2024.
//

#include "Registration.hpp"
#include "../tcp_server/TcpConnection.hpp"

rest::Response rest::Registration::handleRequest(const server::Connection &clientConnection, const rest::Request &request)
{

    if(!decryptBody(clientConnection,request))
    {
        return IRoute::createResponse(boost::beast::http::status::bad_request,"Failed to decrypt request body!",StatusCodes::FAILED_TO_DECRYPT_BODY,request);
    }

    Response response;
    auto userData = parseBody(clientConnection,response,request);
    if(userData.id < 0)
    {
        return response;
    }

    return rest::Response();
}

bool rest::Registration::decryptBody(const server::Connection &clientConnection, const rest::Request &request)
{
    return clientConnection->decrypt(request.body(),decryptedBody);
}
rest::Registration::UserRegistrationData rest::Registration::parseBody(const server::Connection &clientConnection,Response &response,const Request &request)
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
    catch(std::exception &ex)
    {
        userRegistrationData.id = -1;
        response = IRoute::createResponse(boost::beast::http::status::bad_request,ex.what(),StatusCodes::FAILED_TO_PARSE_BODY,request);
    }
    return userRegistrationData;
}
