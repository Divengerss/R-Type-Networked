cmake_minimum_required(VERSION 3.5)
project(r-type_server)

add_executable(${PROJECT_NAME} src/Main.cpp)

include_directories(
    include
    ${PATH_ASIO}
    ${CMAKE_SOURCE_DIR}/srcs/Netcode
    ${CMAKE_SOURCE_DIR}/includes
)
if (WIN32)
    set(PATH_ASIO ${CMAKE_SOURCE_DIR}/vcpkg/packages/asio_x64-windows/include)
endif (WIN32)
if (UNIX)
    set(PATH_ASIO ${CMAKE_SOURCE_DIR}/vcpkg/packages/asio_x64-linux/include)
endif (UNIX)
target_include_directories(${PROJECT_NAME} PRIVATE ${PATH_ASIO})
