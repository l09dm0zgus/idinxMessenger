//
// Created by cx9ps3 on 03.06.2024.
//

#include "Connection.hpp"

app::Connection::Connection(const std::string &newIp, const std::string &newPort) : socket(ioContext), resolver(ioContext), ip(newIp), port(newPort)
{
}

bool app::Connection::connectToServer()
{
    auto endpoint = *resolver.resolve(ip, port).begin();

    try
    {
        socket.connect(endpoint);
        isSuccessfulConnected = true;
        readRSAPublicKey();
    }
    catch (boost::system::error_code &ex)
    {
        std::cout << "Failed to connect!\n What: " << ex.what() << "\n";
    }

    return isSuccessfulConnected;
}

void app::Connection::readRSAPublicKey()
{
    std::array<std::byte, RSA_KEY_LENGTH> bytes{};
    boost::system::error_code errorCode;
    auto readBytes = socket.read_some(boost::asio::buffer(bytes));

    if (readBytes <= 0)
    {
        std::cout << "Failed to get public RSA key!\n";
    }

    if (errorCode)
    {
        std::cout << errorCode.message() << "\n";
        exit(-1488);
    }
    else
    {
        CryptoPP::ByteQueue queue;
        for (int i = 0; i < readBytes; i++)
        {
            queue.Put(static_cast<CryptoPP::byte>(bytes[i]));
        }

        publicKey.Load(queue);
    }
}
