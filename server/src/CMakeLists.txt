cmake_minimum_required(VERSION 3.16)
project(idinxMessenger)

add_executable(${SERVER_TARGET_NAME})
target_sources(${SERVER_TARGET_NAME} PUBLIC main.cpp
        tcp_server/TcpServer.hpp
        tcp_server/TcpServer.cpp
        tcp_server/TcpConnection.hpp
        tcp_server/TcpConnection.cpp
        utils/Logger.hpp
        utils/Logger.cpp
        config/ConfigManager.cpp
        config/ConfigManager.hpp
        config/IConfig.hpp
        config/ServerConfig.cpp
        config/ServerConfig.hpp)
target_link_libraries(${SERVER_TARGET_NAME} pqxx Boost::asio Boost::log_setup Boost::log Boost::beast Boost::json)

if(WIN32)
    target_link_libraries(${SERVER_TARGET_NAME} wsock32 ws2_32)
endif()