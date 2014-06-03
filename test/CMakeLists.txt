INCLUDE(AddFileDependencies)

INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/include")
INCLUDE_DIRECTORIES("${LIBUUID_INCLUDE_DIRS}")
INCLUDE_DIRECTORIES("${ZEROMQ_INCLUDE_DIRS}")
# Locate generated source.
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

FILE(GLOB test_SOURCES "*.cpp")

ADD_EXECUTABLE(dbr_test ${test_SOURCES})
TARGET_LINK_LIBRARIES(dbr_test elm_static fig_static)

INSTALL(TARGETS dbr_test DESTINATION bin)

ADD_CUSTOM_COMMAND(
  DEPENDS ${test_SOURCES}
  COMMAND grep -h "^TEST_CASE" ${test_SOURCES} >${CMAKE_CURRENT_BINARY_DIR}/test_cases.txt
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/test_cases.txt
)

ADD_FILE_DEPENDENCIES(test.cpp ${CMAKE_CURRENT_BINARY_DIR}/test_cases.txt)

MACRO(TEST_CASE name)
  ADD_TEST(${name} ${EXECUTABLE_OUTPUT_PATH}/dbr_test ${name})
ENDMACRO(TEST_CASE)

EXECUTE_PROCESS(COMMAND grep -h "^TEST_CASE" ${test_SOURCES}
  COMMAND sed "s/TEST_CASE(\\(.*\\))/\\1/"
  OUTPUT_STRIP_TRAILING_WHITESPACE
  OUTPUT_VARIABLE TEST_CASES
)

STRING(REGEX REPLACE "(\r?\n)" ";" TEST_CASES ${TEST_CASES})

FOREACH(name ${TEST_CASES})
  MESSAGE(STATUS "TEST_CASE(${name})")
  TEST_CASE(${name})
ENDFOREACH()

# Ensure that each header is self-suficient and can be compiled standalone, so we do not depend on
# the include order.

# Main entry point.

FILE(WRITE ${CMAKE_CURRENT_BINARY_DIR}/stub.cpp "int main(int argc, char* argv[]) {return 0;}")
SET(dbr_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/stub.cpp)

# C header files.

FILE(GLOB dbr_HEADERS RELATIVE ${PROJECT_SOURCE_DIR}/include/dbr
  "${PROJECT_SOURCE_DIR}/include/dbr/*.h")
FOREACH(header ${dbr_HEADERS})

  STRING(REGEX REPLACE ".h\$" ".c" source ${header})
  MESSAGE(STATUS "generate dbr_${source} from ${header}")

  FILE(WRITE ${CMAKE_CURRENT_BINARY_DIR}/dbr_${source} "#include <dbr/${header}>")
  SET(dbr_SOURCES ${dbr_SOURCES} ${CMAKE_CURRENT_BINARY_DIR}/dbr_${source})
ENDFOREACH()

# C++ header files.

FILE(GLOB dbrpp_HEADERS RELATIVE ${PROJECT_SOURCE_DIR}/include/dbrpp
  "${PROJECT_SOURCE_DIR}/include/dbrpp/*.hpp")
FOREACH(header ${dbrpp_HEADERS})

  STRING(REGEX REPLACE ".hpp\$" ".cpp" source ${header})
  MESSAGE(STATUS "generate dbrpp_${source} from ${header}")

  FILE(WRITE ${CMAKE_CURRENT_BINARY_DIR}/dbrpp_${source} "#include <dbrpp/${header}>")
  SET(dbrpp_SOURCES ${dbrpp_SOURCES} ${CMAKE_CURRENT_BINARY_DIR}/dbrpp_${source})
ENDFOREACH()

# Stub executable for compilation.

ADD_EXECUTABLE(dbr_stub ${dbr_SOURCES} ${dbrpp_SOURCES})
