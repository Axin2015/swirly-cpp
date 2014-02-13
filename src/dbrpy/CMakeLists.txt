IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  SET(CMAKE_C_FLAGS "-std=c11 -Wall -Werror -fstrict-aliasing")
ELSEIF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  SET(CMAKE_C_FLAGS "-std=c11 -Wall -Werror -fstrict-aliasing")
ENDIF()

FIND_PACKAGE(PythonLibs REQUIRED)

INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/include")
INCLUDE_DIRECTORIES("${PYTHON_INCLUDE_DIRS}")
INCLUDE_DIRECTORIES("${ZEROMQ_INCLUDE_DIRS}")

FIND_PROGRAM(CYTHON cython)
ADD_CUSTOM_COMMAND(
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/dbrpy.pyx
          ${CMAKE_CURRENT_SOURCE_DIR}/dbr.pxd
          ${CMAKE_CURRENT_SOURCE_DIR}/zmq.pxd
  COMMAND ${CYTHON} -Wextra ${CMAKE_CURRENT_SOURCE_DIR}/dbrpy.pyx
          -o ${CMAKE_CURRENT_BINARY_DIR}/dbrpy.c
  # Hack to work-around missing struct tags.
  COMMAND sed -e 's/DbrIHandler handler\;/struct DbrIHandler handler\;/'
          -e 's/DbrHandlerVtbl vtbl_\;/struct DbrHandlerVtbl vtbl_\;/'
          <${CMAKE_CURRENT_BINARY_DIR}/dbrpy.c
          >${CMAKE_CURRENT_BINARY_DIR}/dbrpy.c.tmp
  COMMAND mv ${CMAKE_CURRENT_BINARY_DIR}/dbrpy.c.tmp ${CMAKE_CURRENT_BINARY_DIR}/dbrpy.c
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/dbrpy.c
  )

SET(dbrpy_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/dbrpy.c)

ADD_LIBRARY(dbrpy MODULE ${dbrpy_SOURCES})
SET_TARGET_PROPERTIES(dbrpy PROPERTIES PREFIX "")
TARGET_LINK_LIBRARIES(dbrpy fig_shared ${PYTHON_LIBRARIES})

INSTALL(TARGETS dbrpy DESTINATION lib)
