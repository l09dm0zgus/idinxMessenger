cmake_minimum_required(VERSION 3.27)
project(idinxMessenger)

set(CMAKE_CXX_STANDARD 20)
set(SERVER_TARGET_NAME idinxServer)

add_executable(${SERVER_TARGET_NAME})
target_sources(${SERVER_TARGET_NAME} PUBLIC main.cpp)