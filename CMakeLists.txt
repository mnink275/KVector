cmake_minimum_required(VERSION 3.15)
project(
    KVector
    DESCRIPTION "SSO Sequence Container"
    LANGUAGES CXX
)

include(FetchContent)

include(cmake/Environment.cmake)
include(cmake/CompileOptions.cmake)
include(cmake/Sanitizers.cmake)

add_subdirectory(core)

add_executable(
    ${PROJECT_NAME} main.cpp
)
target_link_libraries(
    ${PROJECT_NAME} ${PROJECT_NAME}_objs
)

if (CMAKE_BUILD_TYPE MATCHES "Debug")
    include(CTest)
    add_subdirectory(test)
endif()
