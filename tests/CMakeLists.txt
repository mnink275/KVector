FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/7ee260c54921571b18b15049304426fe151c1265.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

add_executable(
  KVecotorTests
  test.cpp
)

target_include_directories(KVecotorTests PRIVATE ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(
  KVecotorTests
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(KVecotorTests)