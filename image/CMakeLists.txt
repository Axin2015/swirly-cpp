INCLUDE(AddFileDependencies)
INCLUDE(FindJava)

FIND_PACKAGE(Java COMPONENTS Runtime)

SET(outfiles)
IF(Java_JAVA_EXECUTABLE AND NOT "${PLANTUML}" STREQUAL "PLANTUML-NOTFOUND" AND DOT)

  MESSAGE(STATUS "plantuml found: ${PLANTUML}")

  FILE(GLOB infiles RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    "${CMAKE_CURRENT_SOURCE_DIR}/*.uml")

  FOREACH(infile ${infiles})

    STRING(REGEX REPLACE ".uml\$" ".png" outfile "${infile}")
    MESSAGE(STATUS "generate ${outfile} from ${infile}")

    ADD_CUSTOM_COMMAND(
      OUTPUT ${PROJECT_BINARY_DIR}/share/doc/html/${outfile}
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      COMMAND ${Java_JAVA_EXECUTABLE} -jar ${PLANTUML} -graphvizdot ${DOT}
              -o ${PROJECT_BINARY_DIR}/share/doc/html ${infile}
      DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${infile})

    SET(outfiles ${outfiles} ${PROJECT_BINARY_DIR}/share/doc/html/${outfile})
  ENDFOREACH()
ENDIF()

ADD_CUSTOM_TARGET(image ALL DEPENDS ${outfiles})
