file(GLOB_RECURSE DIR_SRCS "*.cpp")



foreach(source ${DIR_SRCS})
    get_filename_component(SourceName ${source} NAME_WE)

    set(ExeName ${SourceName}.bin)


    # target
    add_executable(${ExeName} ${SourceName})


    # link library
    target_link_libraries(${ExeName}
            ${PROJECT_SOURCE_DIR}/lib/libmylib.so
            ${LIBRARIES}
                         )


    #设置运行库和连接库
    set_target_properties(${ExeName} PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin"
            LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin"
            RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")


endforeach(source)

