INSTALL(DIRECTORY "${PROJECT_SOURCE_DIR}/include/swirly/"
                  DESTINATION include/swirly
                  FILES_MATCHING
                  PATTERN "*.h"
                  PATTERN "*.hpp"
                  PATTERN "Doxyfile.h" EXCLUDE)
