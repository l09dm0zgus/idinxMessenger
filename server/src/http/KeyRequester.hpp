//
// Created by cx9ps3 on 6/17/24.
//

#pragma once
#include "IRoute.hpp"

namespace server
{
    class TCPConnection;
}

namespace rest
{
    class KeyRequester : public IRoute
    {
    private:
        std::unordered_map<long long, std::shared_ptr<server::TCPConnection>> &connections;
        std::string publicKey;
        long long senderID;
        long long receiverID;
        std::string receiverLogin;
        std::string senderLogin;
        bool parseBody(const std::string_view &body, std::shared_ptr<Response> &response, const Request &request);
        std::string createKeyExchangeMessage();

    protected:
        bool checkSenderIDInDatabase(std::shared_ptr<Response> &response, const Request &request);
        bool checkReceiverLoginInDatabase(std::shared_ptr<Response> &response, const Request &request);
        bool writeMessageToDatabaseIfReceiverOffline(MessageType messageType, std::shared_ptr<Response> &response, const Request &request);
        [[nodiscard]] std::unordered_map<long long, std::shared_ptr<server::TCPConnection>> &getConnection() const noexcept;
        void setPublicKey(const std::string &newPublicKey);
        void setSenderID(long long newSenderID);
        void setReceiverLogin(const std::string newReceiverLogin);
        [[nodiscard]] std::string getPublicKey() const noexcept;
        [[nodiscard]] std::string getReceiverLogin() const noexcept;
        [[nodiscard]] std::string getSenderLogin() const noexcept;
        [[nodiscard]] long long getReceiverID() const noexcept;
        [[nodiscard]] long long getSenderID() const noexcept;
        bool checkIfSameUser(std::shared_ptr<Response> &response, const Request &request);

    public:
        std::shared_ptr<Response> handleRequest(const server::Connection &clientConnection, const Request &request) override;
        explicit KeyRequester(std::unordered_map<long long, std::shared_ptr<server::TCPConnection>> &newConnections);
    };
}// namespace rest
