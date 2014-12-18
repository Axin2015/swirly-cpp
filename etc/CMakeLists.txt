SET(etc_FILES
  FindSwirly.cmake
  FindLibUuid.cmake
  FindZeroMQ.cmake
  sc_example.sql
  sc_mime.types
  sc_nginx.conf
  sc_schema.sql
  swirly.conf
)

INSTALL(FILES ${etc_FILES} DESTINATION etc)
