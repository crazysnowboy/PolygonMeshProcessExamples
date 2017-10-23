
file(GLOB_RECURSE DIR_SRCS "*.cpp")

message(STATUS "crazylog--------------DIR_SRCS=" ${DIR_SRCS} )

# Build each source file independently
add_library(mycpplib SHARED ${DIR_SRCS})

target_link_libraries(mycpplib
        ${PROJECT_SOURCE_DIR}/lib/libmyclib.so
        ${LIBRARIES})


# set properties
set_target_properties(mycpplib PROPERTIES
                        ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib"
                        LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib"
                        RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")


