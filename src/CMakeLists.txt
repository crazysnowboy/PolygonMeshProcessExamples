
file(GLOB_RECURSE DIR_SRCS "*.cpp")

message(STATUS "crazylog--------------DIR_SRCS=" ${DIR_SRCS} )

# Build each source file independently
add_library(mylib SHARED ${DIR_SRCS})

target_link_libraries(mylib
        ${LIBRARIES})


# set properties
set_target_properties(mylib PROPERTIES
                        ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib"
                        LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib"
                        RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")


