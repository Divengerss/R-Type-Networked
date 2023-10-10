cmake_minimum_required(VERSION 3.5)
project(r-type_server)

add_executable(${PROJECT_NAME} src/ECS/Registry.cpp)

include_directories(
    include
    ${PATH_ASIO}
    ${CMAKE_SOURCE_DIR}/srcs/Netcode
    include/Server
    include/ECS
    include/ECS/Components
    include/ECS/Systems
    ${CMAKE_SOURCE_DIR}/includes

)

if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

if (WIN32)
    set(PATH_ASIO ${CMAKE_SOURCE_DIR}/vcpkg/packages/asio_x64-windows/include)
endif (WIN32)
if (UNIX)
    set(PATH_ASIO ${CMAKE_SOURCE_DIR}/vcpkg/packages/asio_x64-linux/include)
endif (UNIX)
if (APPLE)
    set(PATH_ASIO ${CMAKE_SOURCE_DIR}/vcpkg/packages/asio_x64-osx/include)
endif (APPLE)
target_include_directories(${PROJECT_NAME} PRIVATE ${PATH_ASIO})
target_link_libraries(${PROJECT_NAME} -pthread)
