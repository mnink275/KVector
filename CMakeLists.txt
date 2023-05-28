cmake_minimum_required(VERSION 3.15)

project(
    KVector
    VERSION 1.0
    DESCRIPTION "Sequence container"
    LANGUAGES CXX)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")

    include(CTest)

    if(CMAKE_GENERATOR MATCHES "Visual Studio 17 2022")
        set_property(
            DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
    endif()
endif()

include(FetchContent)

add_executable(${PROJECT_NAME} main.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_compile_options(${PROJECT_NAME} PRIVATE "-Wall")

if(BUILD_TESTING)
   add_subdirectory(tests)
endif()