INCLUDE(AddFileDependencies)
INCLUDE(FindJava)

FIND_PACKAGE(Java COMPONENTS Runtime)

SET(outfiles)
IF(DOT)

  MESSAGE(STATUS "Dot found: ${DOT}")

  SET(names
    Ash
    Elm
    Fig
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
              ${lname}_shared)

    SET(outfiles ${outfiles} "${PROJECT_BINARY_DIR}/image/${outfile}")
  ENDFOREACH()

  IF(Java_JAVA_EXECUTABLE AND NOT "${PLANTUML}" STREQUAL "PLANTUML-NOTFOUND")

    MESSAGE(STATUS "Plantuml found: ${PLANTUML}")

    FILE(GLOB infiles RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
      "${CMAKE_CURRENT_SOURCE_DIR}/*.uml")

    FOREACH(infile ${infiles})

      STRING(REGEX REPLACE ".uml\$" ".png" outfile ${infile})
      MESSAGE(STATUS "Generate ${outfile} from ${infile}")

      ADD_CUSTOM_COMMAND(
        OUTPUT "${PROJECT_BINARY_DIR}/image/${outfile}"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMAND "${Java_JAVA_EXECUTABLE}" -jar "${PLANTUML}" -graphvizdot "${DOT}"
                -o "${PROJECT_BINARY_DIR}/image" "${infile}"
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${infile}")

      SET(outfiles ${outfiles} "${PROJECT_BINARY_DIR}/image/${outfile}")
    ENDFOREACH()
  ENDIF()
ENDIF()

ADD_CUSTOM_TARGET(image ALL DEPENDS ${outfiles})
