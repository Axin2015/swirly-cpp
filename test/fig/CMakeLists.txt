FILE(GLOB fig_test_SOURCES "*.cpp")

ADD_EXECUTABLE(swirly_fig_test ${fig_test_SOURCES})
TARGET_LINK_LIBRARIES(swirly_fig_test ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

ADD_TEST(NAME fig_test COMMAND swirly_fig_test)
