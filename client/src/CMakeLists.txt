cmake_minimum_required(VERSION 3.16)
project(idinxMessenger)

add_executable(${CLIENT_TARGET_NAME})
target_sources(${CLIENT_TARGET_NAME} PUBLIC main.cpp
        application/Application.cpp
        application/Application.hpp
        auth/Connection.cpp
        auth/Connection.hpp
        http/HttpRequest.hpp
        auth/Registration.cpp
        auth/Registration.hpp
        database/SQLiteDatabase.cpp
        database/SQLiteDatabase.hpp
        auth/Login.cpp
        auth/Login.hpp
        messaging/PublicKeyExchangeRequest.cpp
        messaging/PublicKeyExchangeRequest.hpp
        messaging/Message.cpp
        messaging/Message.hpp
        messaging/MessageParser.cpp
        messaging/MessageParser.hpp)

target_link_libraries(${CLIENT_TARGET_NAME}
        cryptopp::cryptopp
        SQLiteCpp
        sqlite3
        Boost::asio
        Boost::beast
        Boost::json)


if(WIN32)
    target_link_libraries(${CLIENT_TARGET_NAME} wsock32 ws2_32)
endif()
