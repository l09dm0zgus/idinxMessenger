cmake_minimum_required(VERSION 3.16)
project(idinxMessenger)

add_executable(${SERVER_TARGET_NAME})
target_sources(${SERVER_TARGET_NAME} PUBLIC main.cpp tcp_server/TcpServer.hpp
        tcp_server/TcpServer.cpp
        tcp_server/TcpConnection.hpp
        tcp_server/TcpConnection.cpp
)
target_link_libraries(${SERVER_TARGET_NAME} Boost::asio)

if(WIN32)
    target_link_libraries(${SERVER_TARGET_NAME} wsock32 ws2_32)
endif()