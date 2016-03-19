INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/src/fir")

FILE(GLOB fir_test_SOURCES "*.cpp")

ADD_EXECUTABLE(swirly_fir_test ${fir_test_SOURCES})
TARGET_LINK_LIBRARIES(swirly_fir_test ${tea_LIBRARY} ${fir_LIBRARY})

ADD_TEST(NAME fir_test COMMAND swirly_fir_test)
