INSTALL(DIRECTORY "${PROJECT_SOURCE_DIR}/include/"
                  DESTINATION include
                  FILES_MATCHING
                  PATTERN "*.h"
                  PATTERN "*.hpp"
                  PATTERN "Doxyfile.h" EXCLUDE)
