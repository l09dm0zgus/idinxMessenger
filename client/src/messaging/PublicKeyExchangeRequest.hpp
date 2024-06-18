//
// Created by cx9ps3 on 6/18/24.
//

#pragma once
#include <osrng.h>
#include <rsa.h>
#include <hex.h>
#include <memory>
#include <boost/json.hpp>

namespace auth
{
    class Connection;
}

namespace messaging
{
    class PublicKeyExchangeRequest
    {
    private:
        CryptoPP::AutoSeededRandomPool randomNumberGenerator;
        CryptoPP::InvertibleRSAFunction parameters;
        std::shared_ptr<CryptoPP::RSA::PublicKey> publicKey;
        std::shared_ptr<CryptoPP::RSA::PrivateKey> privateKey;
        std::shared_ptr<auth::Connection> connection;
        void savePrivateKeyToDatabase(const std::string &receiverLogin);
        void generateKeys();
        std::string serializeKeyRequestData(long long senderID,const std::string &receiverLogin);


    public:
        static constexpr int RSA_KEY_LENGTH = 2048;
        explicit PublicKeyExchangeRequest(const std::shared_ptr<auth::Connection> &newConnection);
        boost::json::value createRequest(long long senderID,const std::string &receiverLogin);
        std::shared_ptr<CryptoPP::RSA::PublicKey> getPublicKey();
        std::shared_ptr<CryptoPP::RSA::PrivateKey> getPrivateKey();

        template<class KeyType>
        static std::string rsaKeyToString(const std::shared_ptr<KeyType> &publicKey)
        {
            CryptoPP::ByteQueue queue;
            std::vector<CryptoPP::byte> bytes;
            publicKey->Save(queue);

            for(int i = 0; i < queue.CurrentSize();i++)
            {
                bytes.push_back(queue[i]);
            }


            std::string publicKeyString;
            CryptoPP::HexEncoder encoder(new CryptoPP::StringSink(publicKeyString));
            encoder.Put(bytes.data(),bytes.size());
            encoder.MessageEnd();
            return publicKeyString;
        }

        template<class KeyType>
        static std::shared_ptr<KeyType> decodeRSAKeyFromString(const std::string &rsaKeyString)
        {
            std::vector<CryptoPP::byte> bytes;
            CryptoPP::StringSource ss(rsaKeyString, true,new CryptoPP::HexDecoder(new CryptoPP::VectorSink(bytes)));

            auto publicKey = std::make_shared<KeyType>();

            CryptoPP::ByteQueue queue;
            for (auto byte : bytes)
            {
                queue.Put(byte);
            }

            publicKey->Load(queue);

            return publicKey;
        }
    };
}

