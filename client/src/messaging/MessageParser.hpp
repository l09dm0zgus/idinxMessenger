//
// Created by cx9ps3 on 6/19/24.
//

#pragma once
#include <memory>
#include <iostream>
#include <functional>
#include <boost/json.hpp>

namespace auth
{
    class Connection;
}

namespace messaging
{
    class MessageParser
    {
    public:
        using KeyExchangeConfirmCallback = std::function<bool(const std::string_view&, const std::string_view&, const std::string_view&)>;
        using SentMessageCallback = std::function<void(const std::string_view&, const std::string_view&)>;

    private:
        enum class MessageType
        {
            STANDARD_MESSAGE = 0,
            EXCHANGE_KEYS_MESSAGE
        };

        KeyExchangeConfirmCallback keyExchangeConfirmCallback;
        SentMessageCallback sentMessageCallback;
        long long userID;
        std::shared_ptr<auth::Connection> connection;
        void exchangeKeys(const boost::json::object &object);
        void readMessage(const boost::json::object &object);
    public:
        MessageParser(const std::shared_ptr<auth::Connection> &newConnection,long long newUserID,const KeyExchangeConfirmCallback& newKeyExchangeConfirmCallback,const SentMessageCallback& newSentMessageCallback);
        void parseJSON(const std::string& jsonString);
    };
}

