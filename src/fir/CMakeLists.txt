INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/include")
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/src")

# Remove -Werror due to error: array subscript is above array bounds [-Werror=array-bounds].
IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  SET(CMAKE_C_FLAGS "-std=c11 -Wall -Werror -Wno-array-bounds -fstrict-aliasing -fvisibility=hidden")
ELSEIF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  SET(CMAKE_C_FLAGS "-std=c11 -Wall -Werror -Wno-unused-const-variable  -fstrict-aliasing -fvisibility=hidden")
ENDIF()

FILE(GLOB fir_SOURCES
          sqljourn.c
          sqlmodel.c
          sqlite.c
          sqlite3.c)

#ADD_DEFINITIONS(-DSQLITE_THREADSAFE=0)

ADD_LIBRARY(fir_shared SHARED ${fir_SOURCES})
SET_TARGET_PROPERTIES(fir_shared PROPERTIES OUTPUT_NAME sc_fir)
TARGET_LINK_LIBRARIES(fir_shared fig_shared dl)

ADD_LIBRARY(fir_static STATIC ${fir_SOURCES})
SET_TARGET_PROPERTIES(fir_static PROPERTIES OUTPUT_NAME sc_fir)
TARGET_LINK_LIBRARIES(fir_static fig_static dl)

ADD_LIBRARY(fir_pic STATIC ${fir_SOURCES})
SET_TARGET_PROPERTIES(fir_pic PROPERTIES OUTPUT_NAME sc_fir_pic)
SET_TARGET_PROPERTIES(fir_pic PROPERTIES COMPILE_FLAGS "${CMAKE_SHARED_LIBRARY_C_FLAGS}")
TARGET_LINK_LIBRARIES(fir_pic fig_pic dl)

INSTALL(TARGETS fir_shared DESTINATION lib)
INSTALL(TARGETS fir_static DESTINATION lib)
