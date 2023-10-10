cmake_minimum_required(VERSION 3.5)
project(r-type_client)

if(WIN32)
  set(CPACK_GENERATOR "ZIP")
else()
  set(CPACK_GENERATOR "TGZ")
endif()

set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_SOURCE_IGNORE_FILES .git/ .github/ .vscode/ .mypy_cache/ _CPack_Packages/ ${CMAKE_BINARY_DIR}/ ${PROJECT_BINARY_DIR}/)

include(CPack)

add_executable(${PROJECT_NAME}
        src/Main.cpp
        src/Player.cpp
        src/Background.cpp
        )

include_directories(
    include
    ${CMAKE_SOURCE_DIR}/includes
)

if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

find_package(SFML REQUIRED COMPONENTS graphics window system)
target_link_libraries(${PROJECT_NAME} PRIVATE sfml-graphics sfml-window sfml-system)
