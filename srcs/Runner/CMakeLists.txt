cmake_minimum_required(VERSION 3.5)
project(r-type_runner)

if(WIN32)
  set(CPACK_GENERATOR "ZIP")
else()
  set(CPACK_GENERATOR "TGZ")
endif()

set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_SOURCE_IGNORE_FILES*
    .git/
    .github/
    .vscode/
    .mypy_cache/
    _CPack_Packages/
    .idea/
    R-Type-documentations/
    mkdocs-material/
    build/
    .logo.png
    ${CMAKE_BINARY_DIR}/
    ${PROJECT_BINARY_DIR}/
)
set(CPACK_COMPONENTS_ALL ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(${PROJECT_NAME}
    src/Main.cpp
)

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${CMAKE_BINARY_DIR}/bin
)

install(FILES
    ${CMAKE_SOURCE_DIR}/Release/client.cfg
    DESTINATION .)
install(DIRECTORY
    ${CMAKE_SOURCE_DIR}/Release/assets
    DESTINATION .)

include(CPack)

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

find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
    $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
)