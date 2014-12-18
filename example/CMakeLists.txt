INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/include")
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/src")
INCLUDE_DIRECTORIES("${LIBUUID_INCLUDE_DIRS}")
INCLUDE_DIRECTORIES("${ZEROMQ_INCLUDE_DIRS}")

ADD_EXECUTABLE(sc_perf perf.c)
TARGET_LINK_LIBRARIES(sc_perf fir_static pthread)

ADD_EXECUTABLE(sc_jdtoymd jdtoymd.c)
TARGET_LINK_LIBRARIES(sc_jdtoymd ash_static)

ADD_EXECUTABLE(sc_ymdtojd ymdtojd.c)
TARGET_LINK_LIBRARIES(sc_ymdtojd ash_static)

ADD_EXECUTABLE(sc_dump dump.cpp)
TARGET_LINK_LIBRARIES(sc_dump fir_static pthread)

ADD_EXECUTABLE(sc_rest rest.cpp)
TARGET_LINK_LIBRARIES(sc_rest fir_static)

ADD_EXECUTABLE(sc_skel skel.cpp)
TARGET_LINK_LIBRARIES(sc_skel fig_static pthread)
