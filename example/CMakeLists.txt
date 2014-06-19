INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/include")
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/src")
INCLUDE_DIRECTORIES("${LIBUUID_INCLUDE_DIRS}")
INCLUDE_DIRECTORIES("${ZEROMQ_INCLUDE_DIRS}")

ADD_EXECUTABLE(dbr_perf perf.c)
TARGET_LINK_LIBRARIES(dbr_perf fir_static pthread)

ADD_EXECUTABLE(dbr_jdtoymd jdtoymd.c)
TARGET_LINK_LIBRARIES(dbr_jdtoymd ash_static)

ADD_EXECUTABLE(dbr_ymdtojd ymdtojd.c)
TARGET_LINK_LIBRARIES(dbr_ymdtojd ash_static)

ADD_EXECUTABLE(dbr_dump dump.cpp)
TARGET_LINK_LIBRARIES(dbr_dump fir_static pthread)

ADD_EXECUTABLE(dbr_rest rest.cpp)
TARGET_LINK_LIBRARIES(dbr_rest fir_static)

ADD_EXECUTABLE(dbr_skel skel.cpp)
TARGET_LINK_LIBRARIES(dbr_skel fig_static pthread)
