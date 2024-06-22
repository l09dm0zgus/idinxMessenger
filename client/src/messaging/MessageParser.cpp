//
// Created by cx9ps3 on 6/19/24.
//

#include "MessageParser.hpp"
#include "../database/SQLiteDatabase.hpp"
#include "Message.hpp"
#include "PublicKeyExchangeRequest.hpp"

void messaging::MessageParser::parseJSON(const std::string &jsonString)
{
    try
    {
        auto object = boost::json::parse(jsonString).as_object();
        if(object.if_contains("request_key"))
        {
            exchangeKeys(object);
        }
        else if(object.if_contains("user_message"))
        {
            readMessage(object);
        }
    }
    catch(const boost::system::system_error &ex)
    {
        return;
    }
}

void messaging::MessageParser::exchangeKeys(const boost::json::object &object)
{
    std::string_view sender,receiver,senderPublicKey;
    for(const auto& [key,value] : object)
    {
        sender = value.at("sender").as_string();
        receiver = value.at("receiver").as_string();
        senderPublicKey = value.at("sender_public_key").as_string();
    }

    db::SQLiteDatabase database;
    auto query = database.query("SELECT * FROM key_exchange_requests WHERE receiver_login=?");
    query.bind(1,sender.data());

    if(query.executeStep())
    {
        auto updateQuery = database.query("UPDATE  key_exchange_requests SET receiver_public_key=? WHERE receiver_login=?");
        updateQuery.bind(1,senderPublicKey.data());
        updateQuery.bind(2,sender.data());
        updateQuery.exec();
    }
    else
    {
        if(keyExchangeConfirmCallback(receiver,sender,senderPublicKey))
        {
            PublicKeyExchangeRequest publicKeyExchangeRequest(connection);
            publicKeyExchangeRequest.createRequest(userID,sender.data());

            auto updateQuery = database.query("UPDATE  key_exchange_requests SET receiver_public_key=? WHERE receiver_login=?");
            updateQuery.bind(1,senderPublicKey.data());
            updateQuery.bind(2,sender.data());
            updateQuery.exec();
        }
    }
}

messaging::MessageParser::MessageParser(const std::shared_ptr<auth::Connection> &newConnection,long long newUserID,const KeyExchangeConfirmCallback& newKeyExchangeConfirmCallback,const SentMessageCallback& newSentMessageCallback) : connection(newConnection),userID(newUserID), keyExchangeConfirmCallback(newKeyExchangeConfirmCallback),sentMessageCallback(newSentMessageCallback)
{
}

void messaging::MessageParser::readMessage(const boost::json::object &object)
{
    std::string_view sender,receiver,senderPublicKey,encryptedMessage,encryptedKey;
    for(const auto& [key,value] : object)
    {
        sender = value.at("sender").as_string();
        receiver = value.at("receiver").as_string();
        senderPublicKey = value.at("sender_public_key").as_string();
        encryptedMessage = value.at("message").as_string();
        encryptedKey = value.at("encrypted_key").as_string();
    }
    auto decryptedMessage = Message::decryptMessage(encryptedMessage,encryptedKey,sender);
    sentMessageCallback(sender,decryptedMessage);

    db::SQLiteDatabase database;
    long long conservationID {0};

    auto selectQuery = database.query("SELECT conversation_id FROM messages WHERE (receiver=? AND sender=?) OR (sender=? AND receiver=?)");
    selectQuery.bind(1,receiver.data());
    selectQuery.bind(2,sender.data());
    selectQuery.bind(3,receiver.data());
    selectQuery.bind(4,sender.data());

    if(selectQuery.executeStep())
    {
        conservationID = selectQuery.getColumn("conversation_id").getInt64();
    }
    else
    {
        CryptoPP::AutoSeededRandomPool rng;
        CryptoPP::Integer id(rng, 0, std::numeric_limits<long>::max());
        conservationID = id.ConvertToLong();
    }

    auto insertQuery = database.query("INSERT INTO messages (sender,receiver,message,conversation_id,next_public_key) VALUES (?,?,?,?,?)");
    insertQuery.bind(1,sender.data());
    insertQuery.bind(2,receiver.data());
    insertQuery.bind(3,decryptedMessage);
    insertQuery.bind(4,static_cast<long>(conservationID));
    insertQuery.bind(5,senderPublicKey.data());
    insertQuery.exec();
}
