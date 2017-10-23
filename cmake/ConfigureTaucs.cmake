#Taucs
add_definitions(-DTAUCS_CORE_GENERAL)
add_definitions(-DTAUCS_DOUBLE_IN_BUILD)
add_definitions(-DTAUCS_CORE_DOUBLE)


#add_definitions(-DNOMETIS)

list(APPEND LIBRARIES
        -lmetis
        /usr/lib/x86_64-linux-gnu/libf2c.a
        -lm)