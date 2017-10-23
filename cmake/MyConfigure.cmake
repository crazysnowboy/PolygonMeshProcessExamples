
list(APPEND LIBRARIES  -lopenblas -lcurl
        /usr/local/lib/libOpenMeshCore.so
        /usr/local/lib/libOpenMeshTools.so
        ) #for libTracker


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -w")
set(CMAKE_C_FLAGS " -w")

set(DCMAKE_CXX_COMPILER g++-5)
set(CMAKE_C_COMPILER gcc-5)

find_package(Boost 1.50.0 COMPONENTS system filesystem program_options REQUIRED)
if(Boost_FOUND)
    message(STATUS "Boost found at ${Boost_INCLUDE_DIRS}")
    list(APPEND LIBRARIES ${Boost_LIBRARIES})
else(Boost_FOUND)
    message(FATAL_ERROR "Boost not found")
endif()

find_package(Pangolin 0.1 REQUIRED)
if(Pangolin_FOUND)
    message(STATUS "Pangolin found at ${Pangolin_INCLUDE_DIRS}")
    include_directories(${Pangolin_INCLUDE_DIRS})
    list(APPEND LIBRARIES  ${Pangolin_LIBRARIES})
else(Pangolin_FOUND)
    message(FATAL_ERROR "Pangolin not found")
endif()


#FIND_PATH(ANTTWEAKBAR_INCLUDE_DIR AntTweakBar.h ${ANTTWEAKBAR_ROOT}/include)
#FIND_LIBRARY(ANTTWEAKBAR_LIBRARY AntTweakBar ${ANTTWEAKBAR_ROOT}/lib)
#list(APPEND LIBRARIES ${ANTTWEAKBAR_LIBRARY})

