INCLUDE(AddFileDependencies)

SET(outfiles)
IF(DOT_EXECUTABLE)

  MESSAGE(STATUS "Dot found: ${DOT_EXECUTABLE}")

  SET(names
    Ash
    Elm
    Fig
    Fir
  )

  FOREACH(name ${names})

    SET(outfile ${name}Depend.png)

    STRING(TOLOWER ${name} lname)
    ADD_CUSTOM_COMMAND(
      OUTPUT "${PROJECT_BINARY_DIR}/image/${outfile}"
      WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
      COMMAND CXX="${CMAKE_CXX_COMPILER}" "${PERL}" etc/dotdepend.pl ${lname}
              | dot -Tpng >"${PROJECT_BINARY_DIR}/image/${outfile}"
      DEPENDS "${PROJECT_SOURCE_DIR}/etc/dotdepend.pl"
              ${lname}_static)

    SET(outfiles ${outfiles} "${PROJECT_BINARY_DIR}/image/${outfile}")
  ENDFOREACH()

ENDIF()

ADD_CUSTOM_TARGET(image ALL DEPENDS ${outfiles})
