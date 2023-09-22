cmake_minimum_required(VERSION 3.5)
project(r-type_client)

add_executable(${PROJECT_NAME}
        src/Main.cpp
        src/Player.cpp
        )

include_directories(include)
find_package(SFML REQUIRED COMPONENTS graphics window system)
target_link_libraries(${PROJECT_NAME} PRIVATE sfml-graphics sfml-window sfml-system)
