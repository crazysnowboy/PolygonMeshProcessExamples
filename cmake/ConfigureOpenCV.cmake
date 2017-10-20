#--- (OBSOLETE) find it with PkgConfig
find_package(PkgConfig REQUIRED)
pkg_check_modules(OPENCV REQUIRED opencv)

list(APPEND LIBRARIES ${OPENCV_LDFLAGS})

message("-------------OPENCV_LDFLAGS------------start----")
message(${OPENCV_LDFLAGS})
message("-------------OPENCV_LDFLAGS------------end----")


include_directories(${OPENCV_INCLUDE_DIRS})

message("-------------OPENCV_INCLUDE_DIRS------------start----")
message(${OPENCV_INCLUDE_DIRS})
message("-------------OPENCV_INCLUDE_DIRS------------end----")

#--- (OBSOLETE) find it with PkgConfig
#find_package(PkgConfig REQUIRED)
#pkg_check_modules(OPENCV REQUIRED opencv)
#list(APPEND LIBRARIES ${OPENCV_LDFLAGS})
#
#
#message("-------------LIBRARIES-------------start----")
#message(${LIBRARIES})
#message("-------------LIBRARIES------------end----")
#
#
#include_directories(${OPENCV_INCLUDE_DIRS})
#
#message("-------------OPENCV_INCLUDE_DIRS------------start----")
#message(${OPENCV_INCLUDE_DIRS})
#message("-------------OPENCV_INCLUDE_DIRS------------end----")