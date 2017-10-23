CMAKE_MINIMUM_REQUIRED(VERSION 2.6) #cmake 版本限定
PROJECT(PolygonMeshProcession)  #工程名称

include(cmake/ConfigureOpenCV.cmake)
include(cmake/ConfigureTaucs.cmake)
include(cmake/MyConfigure.cmake)
include(cmake/ConfigureOpengl.cmake)
#set(CMAKE_VERBOSE_MAKEFILE ON)

#set(CMAKE_BUILD_TYPE Release) # 等同于 cmake -DCMAKE_BUILD_TYPE=release


# ---Includes
set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/include/glfw-3.1.2/include/GLFW
        ${PROJECT_SOURCE_DIR}/include/pangolin-ui
        ${PROJECT_SOURCE_DIR}/include/taucs
        ${PROJECT_SOURCE_DIR}/include/OpenMesh
        /usr/include/event2

        ${PROJECT_SOURCE_DIR}/include/polygon_mesh_processor)

include_directories(${INCLUDE_DIR})


message(STATUS "crazylog--------------LIBRARIES=" ${LIBRARIES} )

add_subdirectory(${PROJECT_SOURCE_DIR}/src/c)
add_subdirectory(${PROJECT_SOURCE_DIR}/src/cpp)
add_subdirectory(${PROJECT_SOURCE_DIR}/main)

