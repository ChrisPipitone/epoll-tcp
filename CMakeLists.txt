cmake_minimum_required(VERSION 3.14)

project(epoll-tcp VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

FetchContent_Declare(
    FTXUI
    GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI
    GIT_TAG        v7.0.3
)
FetchContent_MakeAvailable(FTXUI)

add_executable(server
  src/server/main.cpp
)
add_executable(client
  src/client/main.cpp
)
target_include_directories(server PRIVATE src)
target_include_directories(client PRIVATE src)

# Server
target_compile_options(server PRIVATE -Wall -Wextra -Wpedantic -Werror)
target_link_libraries(server PRIVATE ftxui)

# Client
target_compile_options(client PRIVATE -Wall -Wextra -Wpedantic -Werror)
