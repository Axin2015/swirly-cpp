SET(etc_FILES
  FindDoobry.cmake
  FindLibUuid.cmake
  FindZeroMQ.cmake
  dbr_example.sql
  dbr_mime.types
  dbr_nginx.conf
  dbr_schema.sql
  doobry.conf
)

INSTALL(FILES ${etc_FILES} DESTINATION etc)
