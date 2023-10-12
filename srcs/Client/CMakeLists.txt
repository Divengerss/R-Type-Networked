cmake_minimum_required(VERSION 3.5)
project(r-type_client)

add_executable(${PROJECT_NAME}
        src/Main.cpp
        src/Player.cpp
        src/Background.cpp
        )

include_directories(
    include
    ${CMAKE_SOURCE_DIR}/includes
    ${CMAKE_SOURCE_DIR}/ECS
    ${CMAKE_SOURCE_DIR}/ECS/Components
    ${CMAKE_SOURCE_DIR}/ECS/Systems
)

if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

find_package(SFML REQUIRED COMPONENTS graphics window system)
target_link_libraries(${PROJECT_NAME} PRIVATE sfml-graphics sfml-window sfml-system)
