FILE(GLOB elm_test_SOURCES "*.cpp")

ADD_EXECUTABLE(swirly_elm_test ${elm_test_SOURCES})
TARGET_LINK_LIBRARIES(swirly_elm_test ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

ADD_TEST(NAME elm_test COMMAND swirly_elm_test)
