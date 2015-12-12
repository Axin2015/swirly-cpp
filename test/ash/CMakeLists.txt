FILE(GLOB ash_test_SOURCES "*.cpp")

ADD_EXECUTABLE(swirly_ash_test ${ash_test_SOURCES})
TARGET_LINK_LIBRARIES(swirly_ash_test ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

ADD_TEST(NAME ash_test COMMAND swirly_ash_test)
