cmake_minimum_required(VERSION 3.14)
project(remote_sync)

set(CMAKE_CXX_STANDARD 17)

add_executable(remote_sync main.cpp server/MirrorServer.cpp)

target_link_libraries(remote_sync pthread inotify-cpp boost_program_options ssh)
