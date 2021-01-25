cmake_minimum_required(VERSION 3.17)
project(asio_server_test)

set(CMAKE_CXX_STANDARD 14)

find_package(Boost REQUIRED)
message(${Boost_INCLUDE_DIRS})
add_executable(asio_server_test main.cpp)

target_link_libraries(asio_server_test PRIVATE pthread)
target_include_directories(asio_server_test PRIVATE ${Boost_INCLUDE_DIRS})
