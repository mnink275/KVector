add_library(
    ${PROJECT_NAME}_objs OBJECT
    "./include/KVector.hpp"
    "./include/Iterator.hpp"
)
target_include_directories(
    ${PROJECT_NAME}_objs PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include
)
