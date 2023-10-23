cmake_minimum_required(VERSION 3.12)
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")
if (WIN32)
    execute_process(
        COMMAND ${CMAKE_SOURCE_DIR}/vcpkg/bootstrap-vcpkg.bat
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/vcpkg
    )
    execute_process(
        COMMAND ${CMAKE_SOURCE_DIR}/vcpkg/vcpkg.exe install asio sfml
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/vcpkg
    )
else()
    execute_process(
        COMMAND ${CMAKE_SOURCE_DIR}/vcpkg/bootstrap-vcpkg.sh
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/vcpkg
    )
    execute_process(
        COMMAND ${CMAKE_SOURCE_DIR}/vcpkg/vcpkg install asio sfml
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/vcpkg
    )
endif()
project("R-Type")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Release)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(srcs/Server)
add_subdirectory(srcs/Client)
