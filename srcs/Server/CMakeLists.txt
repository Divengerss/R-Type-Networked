cmake_minimum_required(VERSION 3.5)
project(r-type_server)

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

add_executable(${PROJECT_NAME} src/Main.cpp)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION .)
install(FILES
    ${CMAKE_SOURCE_DIR}/Release/server.cfg
    DESTINATION .)

include(CPack)

if (WIN32)
    set(PATH_ASIO ${CMAKE_SOURCE_DIR}/vcpkg/packages/asio_x64-windows/include)
endif (WIN32)
if (UNIX)
    set(PATH_ASIO ${CMAKE_SOURCE_DIR}/vcpkg/packages/asio_x64-linux/include)
endif (UNIX)
if (APPLE)
    set(PATH_ASIO ${CMAKE_SOURCE_DIR}/vcpkg/packages/asio_x64-osx/include)
endif (APPLE)

include_directories(
    include
    ${PATH_ASIO}
    ${CMAKE_SOURCE_DIR}/includes
)

if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ${PATH_ASIO})
target_link_libraries(${PROJECT_NAME} -pthread)
