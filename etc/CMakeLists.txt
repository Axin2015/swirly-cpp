SET(etc_FILES
  dbr_example.sql
  dbr_schema.sql
  doobry.conf
  FindDoobry.cmake
)

INSTALL(FILES ${etc_FILES} DESTINATION etc)
