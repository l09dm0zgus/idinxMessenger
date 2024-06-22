//
// Created by cx9ps3 on 6/18/24.
//

#pragma once
#include <rsa.h>
#include <boost/json.hpp>
#include "PublicKeyExchangeRequest.hpp"
#include "../database/SQLiteDatabase.hpp"
#include <iostream>

namespace auth
{
    class Connection;
}

namespace messaging
{
    class Message
    {
    private:
        std::string receiverLogin;
        std::string senderLogin;
        long long senderID;
        std::string aesKey;
        std::shared_ptr<auth::Connection> connection;
        std::shared_ptr<CryptoPP::RSA::PublicKey> nextPublicKey;
        std::shared_ptr<CryptoPP::RSA::PrivateKey> nextPrivateKey;
        std::string encryptUserMessage(const std::string &userMessage);
        std::string encryptAESKey();
        std::string serializeUserMessage(const std::string &userMessage);
        void storeToDatabase(const std::string &userMessage);
        static auto generateNextRSAKeys();
        static std::string decryptAESKey(const std::string_view &encryptedAESKey, const std::string_view &senderLogin);
        static std::string decryptMessage(const std::string_view &encryptedMessage,const std::string &aesKey);

    public:
        template<class KeyType>
        static std::shared_ptr<KeyType> getLastKeyFromDatabase(const std::string &senderLogin)
        {
            std::string_view keyExchangeQuery,messagesQuery,messagesColumnName,keyExchangeColumnName;
            if constexpr (std::is_same_v<KeyType,CryptoPP::RSA::PublicKey>)
            {
                keyExchangeQuery = "SELECT receiver_public_key FROM key_exchange_requests WHERE receiver_login=?";
                messagesQuery = "SELECT next_public_key FROM messages WHERE sender=? ORDER BY id DESC LIMIT 1";
                keyExchangeColumnName = "receiver_public_key";
                messagesColumnName = "next_public_key";
            }
            else if constexpr (std::is_same_v<KeyType,CryptoPP::RSA::PrivateKey>)
            {
                keyExchangeQuery = "SELECT private_key FROM key_exchange_requests WHERE receiver_login=?";
                messagesQuery = "SELECT next_private_key FROM messages WHERE receiver=? ORDER BY id DESC LIMIT 1";
                keyExchangeColumnName = "private_key";
                messagesColumnName = "next_private_key";
            }
            else
            {
                static_assert(false,"KeyType must  be only CryptoPP::RSA::PublicKey or CryptoPP::RSA::PrivateKey");
            }

            db::SQLiteDatabase database;
            try
            {
                auto selectQuery = database.query(messagesQuery.data());
                selectQuery.bind(1,senderLogin);

                if(selectQuery.executeStep())
                {
                    return PublicKeyExchangeRequest::decodeRSAKeyFromString<KeyType>(selectQuery.getColumn(messagesColumnName.data()));
                }
                else
                {
                    auto query = database.query(keyExchangeQuery.data());
                    query.bind(1,senderLogin);
                    if(query.executeStep())
                    {
                        return PublicKeyExchangeRequest::decodeRSAKeyFromString<KeyType>(query.getColumn(keyExchangeColumnName.data()));
                    }
                    return nullptr;
                }
            }
            catch (SQLite::Exception &ex)
            {
                std::cout << "Error: " << ex.what() << "\n";
                exit(-1488);
            }
        }

        static std::string decryptMessage(const std::string_view &encryptedMessage,const std::string_view &encryptedAESKey, const std::string_view& senderLogin);
        Message(const std::shared_ptr<auth::Connection> &newConnection,const std::string &newReceiverLogin, const std::string &newSenderLogin ,long long newSenderID);
        boost::json::value send(const std::string &userMessage);
    };
}

