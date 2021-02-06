cmake_minimum_required(VERSION 3.14)

project(uvcc LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG")

find_package(PkgConfig REQUIRED QUIET)

pkg_check_modules(uv
    REQUIRED QUIET
    libuv
    IMPORTED_TARGET
)

add_executable(${PROJECT_NAME}
    src/main.cc
    include/uvcc/network.h
#    include/uvcc/handle.h
)

target_include_directories(${PROJECT_NAME} PRIVATE "include")
target_link_libraries(${PROJECT_NAME} PRIVATE PkgConfig::uv)
