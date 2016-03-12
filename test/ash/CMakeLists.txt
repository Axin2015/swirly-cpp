INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/src/ash")

FILE(GLOB ash_test_SOURCES "*.cpp")

ADD_EXECUTABLE(swirly_ash_test ${ash_test_SOURCES})
TARGET_LINK_LIBRARIES(swirly_ash_test ash_shared)

ADD_TEST(NAME ash_test COMMAND swirly_ash_test)
