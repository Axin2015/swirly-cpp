# Mongoose does not compile with strict aliasing.
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-strict-aliasing")

FILE(GLOB swirlyd_SOURCES "*.cpp")

ADD_EXECUTABLE(swirlyd ${swirlyd_SOURCES} "${PROJECT_SOURCE_DIR}/contrib/mongoose.c")
TARGET_LINK_LIBRARIES(swirlyd fir_shared ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_PROGRAM_OPTIONS_LIBRARY} ${Boost_SYSTEM_LIBRARY})
INSTALL(TARGETS swirlyd DESTINATION bin)
