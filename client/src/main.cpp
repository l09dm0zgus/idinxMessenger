//
// Created by cx9ps3 on 06.02.2024.
//

#include <boost/asio.hpp>
#include <cstdlib>
#include <cstring>
#include <iostream>

using boost::asio::ip::tcp;

static constexpr int BUFFER_SIZE = 1024;

int main(int argc, char *argv[])
{
    try
    {

        boost::asio::io_context io_context;
        tcp::socket s(io_context);
        s.connect(tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 1488));

        std::cout << "Enter message: ";
        std::string inputBuffer;
        std::cin >> inputBuffer;

        boost::asio::write(s, boost::asio::buffer(inputBuffer));

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