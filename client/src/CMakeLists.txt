cmake_minimum_required(VERSION 3.16)
project(idinxMessenger)

add_executable(${CLIENT_TARGET_NAME})
target_sources(${CLIENT_TARGET_NAME} PUBLIC main.cpp)

target_link_libraries(${CLIENT_TARGET_NAME} Boost::asio)

if(WIN32)
    target_link_libraries(${CLIENT_TARGET_NAME} wsock32 ws2_32)
endif()
