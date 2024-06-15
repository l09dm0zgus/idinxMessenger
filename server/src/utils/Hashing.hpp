//
// Created by cx9ps3 on 6/14/24.
//
#pragma once
#include <string>
#include <blake2.h>
#include <hex.h>
#include <osrng.h>
#include <pwdbased.h>

namespace utils
{
    static constexpr int SALT_SIZE = 16;
    std::string hashPassword(const std::string &password)
    {
        CryptoPP::SecByteBlock salt(SALT_SIZE);
        CryptoPP::OS_GenerateRandomBlock(true,salt,SALT_SIZE);
        CryptoPP::byte derived[CryptoPP::BLAKE2b::DIGESTSIZE];
        CryptoPP::byte unused = 0;
        CryptoPP::PKCS5_PBKDF2_HMAC<CryptoPP::BLAKE2b> pbkdf;
        pbkdf.DeriveKey(derived, sizeof(derived), unused, reinterpret_cast<const CryptoPP::byte*>(password.c_str()), password.size(), salt, SALT_SIZE, 1024, 0.0f);

        std::string passwordHashed;
        CryptoPP::HexEncoder passwordEncoder(new CryptoPP::StringSink(passwordHashed));
        passwordEncoder.Put(derived, sizeof(derived));
        passwordEncoder.MessageEnd();

        std::string saltString;
        CryptoPP::HexEncoder saltEncoder(new CryptoPP::StringSink(saltString));
        saltEncoder.Put(salt,SALT_SIZE);
        saltEncoder.MessageEnd();

        return saltString + "$" + passwordHashed;
    }
}
