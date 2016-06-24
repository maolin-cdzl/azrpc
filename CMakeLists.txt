CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)

project(azrpc)
set(AZRPC_VERSION_MAJOR 0)
set(AZRPC_VERSION_MINOR 1)
set(AZRPC_VERSION_PATCH 0)

set(CMAKE_C_FLAGS "-std=c99 -fPIC ${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS "-std=c++11 -fPIC ${CMAKE_CXX_FLAGS}")

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake_modules)
find_package(ProtobufPlugin REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
add_subdirectory(src)
add_subdirectory(test)

