#openni 1.5
set(OpenGL_INC_DIR /usr/include/GL)
set(OpenGL_LIB_DIR /usr/lib/x86_64-linux-gnu)
include_directories("${OpenGL_INC_DIR}")
link_directories("${OpenGL_LIB_DIR}")
set(    Opengl_LIB
        ${OpenGL_LIB_DIR}/libglut.so
        ${OpenGL_LIB_DIR}/libGLEW.so
        ${OpenGL_LIB_DIR}/libGLU.so
        ${OpenGL_LIB_DIR}/libGL.so
        ${OpenGL_LIB_DIR}/libglfw.so
        )
list(APPEND LIBRARIES ${Opengl_LIB})


