INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/src/elm")

FILE(GLOB elm_test_SOURCES "*.cpp")

ADD_EXECUTABLE(swirly_elm_test ${elm_test_SOURCES})
TARGET_LINK_LIBRARIES(swirly_elm_test ${tea_LIBRARY} ${elm_LIBRARY})

ADD_TEST(NAME elm_test COMMAND swirly_elm_test)
