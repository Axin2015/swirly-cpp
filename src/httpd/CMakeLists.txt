# Mongoose does not compile with strict aliasing.
SET(COMMON_WARN "-Wall -Werror -Wextra -Wno-unused-parameter -Wno-unused-variable")
SET(COMMON_FLAGS "-fvisibility=hidden")

IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  SET(CMAKE_C_FLAGS "-std=c11 ${COMMON_FLAGS} ${COMMON_WARN}")
ELSEIF("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
  SET(CMAKE_C_FLAGS "-std=c11 ${COMMON_FLAGS} ${COMMON_WARN}")
ENDIF()

FILE(GLOB httpd_SOURCES "*.cpp")
ADD_EXECUTABLE(swirly_httpd ${httpd_SOURCES} mongoose.c)
TARGET_LINK_LIBRARIES(swirly_httpd elm_shared)
INSTALL(TARGETS swirly_httpd DESTINATION bin)
