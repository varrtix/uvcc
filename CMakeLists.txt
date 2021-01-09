cmake_minimum_required(VERSION 3.5)

project(uvcc LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED QUIET)

pkg_check_modules(uv
    REQUIRED QUIET
    libuv
    IMPORTED_TARGET
)

add_executable(${PROJECT_NAME}
    utilities.h
    exception.h
    version.h
    event-loop.h
    main.cc
)

target_link_libraries(${PROJECT_NAME} PRIVATE PkgConfig::uv)
