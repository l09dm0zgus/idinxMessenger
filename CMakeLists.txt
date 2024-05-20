cmake_minimum_required(VERSION 3.16)
project(idinxMessenger)

set(CMAKE_CXX_STANDARD 17)
set(SERVER_TARGET_NAME idinxServer)
set(CLIENT_TARGET_NAME idinxClient)


add_subdirectory(submodules/boost EXCLUDE_FROM_ALL)
add_subdirectory(submodules/cryptopp-cmake)
add_subdirectory(server)
add_subdirectory(client)
