cmake_minimum_required(VERSION 3.5)
project(r-type-engine)

include_directories(
    include
    ${CMAKE_SOURCE_DIR}/ECS
    ${CMAKE_SOURCE_DIR}/Component
    ${CMAKE_SOURCE_DIR}/Systems
)

add_library(${PROJECT_NAME} SHARED
    src/Engine.cpp
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "RTEngine"
    PREFIX ""
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE MY_DYNAMIC_LIBRARY_EXPORT)
endif()

install(
    TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_BINARY_DIR}/lib
    ARCHIVE DESTINATION ${CMAKE_BINARY_DIR}/lib
    RUNTIME DESTINATION ${CMAKE_BINARY_DIR}/bin
)