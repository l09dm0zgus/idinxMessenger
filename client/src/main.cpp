//
// Created by cx9ps3 on 06.02.2024.
//

#include "http/HttpRequest.hpp"
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/websocket.hpp>
#include <iostream>

using boost::asio::ip::tcp;

static constexpr int BUFFER_SIZE = 1024;

int main(int argc, char *argv[])
{
    try
    {

        boost::asio::io_context ioContext;
        tcp::socket s(ioContext);
        boost::asio::ip::tcp::resolver resolver(ioContext);

        boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve("localhost", std::to_string(1488)).begin();
        s.connect(endpoint);

        rest::HttpRequest<rest::Method::POST> request("huy/1", "localhost", "application/json", "{jopa = 1488}");

        boost::beast::http::write(s, *request.getBeastRequestObject());

        char reply[BUFFER_SIZE];
        auto replyLength = boost::asio::read(s, boost::asio::buffer(reply));

        std::cout << "Reply is: ";
        std::cout << reply;
        std::cout << "\n";
    }
    catch (std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}