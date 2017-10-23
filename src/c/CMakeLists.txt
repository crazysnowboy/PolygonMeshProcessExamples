
file(GLOB_RECURSE DIR_SRCS "*.c")

#message(STATUS "c--------------DIR_SRCS=" ${DIR_SRCS} )

# Build each source file independently
add_library(myclib SHARED ${DIR_SRCS})

target_link_libraries(myclib
                     ${LIBRARIES})


# set properties
set_target_properties(myclib PROPERTIES
                        ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib"
                        LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib"
                        RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")


