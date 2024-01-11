add_compile_options(-Wall -Wextra -Wpedantic)
if (CMAKE_BUILD_TYPE MATCHES "Debug")
add_compile_options(-g)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
