# Mongoose does not compile with strict aliasing.
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-strict-aliasing")

# Use epoll() on Linux.
IF(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  ADD_DEFINITIONS(-DMG_MGR_EV_MGR=1)
ENDIF()

FILE(GLOB swirlyd_SOURCES "*.cpp")

ADD_EXECUTABLE(swirlyd ${swirlyd_SOURCES} "${PROJECT_SOURCE_DIR}/contrib/mongoose.c")
TARGET_LINK_LIBRARIES(swirlyd ${fir_LIBRARY} ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})
INSTALL(TARGETS swirlyd DESTINATION bin)
