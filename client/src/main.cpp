//
// Created by cx9ps3 on 06.02.2024.
//

#include "http/HttpRequest.hpp"
#include "application/Application.hpp"
static constexpr int BUFFER_SIZE = 1024;

auto main(int argc, char *argv[]) -> int
{

    app::Application app;
    app.run();

    /*
    try
    {



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
     */

    return 0;
}