cmake_minimum_required(VERSION 3.5)
project(r-type-engine)

set(SOURCES
    src/Engine.cpp
)

include_directories(
    include
    ${CMAKE_SOURCE_DIR}/ECS
    ${CMAKE_SOURCE_DIR}/Component
    ${CMAKE_SOURCE_DIR}/Systems
)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES
    VERSION 1.0
    SOVERSION 1
    OUTPUT_NAME "RTEngine"
    PREFIX ""
)

if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE MY_DYNAMIC_LIBRARY_EXPORT)
endif()

install(
    TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)