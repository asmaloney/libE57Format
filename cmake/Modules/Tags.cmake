# This file defines the variables
# ${PROJECT_NAME}_BUILD_VERSION
# ${PROJECT_NAME}_BUILD_TAG

# try to find subversion revision number
execute_process(
    COMMAND svnversion
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE ${PROJECT_NAME}_BUILD_VERSION
)
if (NOT ${PROJECT_NAME}_BUILD_VERSION)
    set(${PROJECT_NAME}_BUILD_VERSION "unknown")
else()
    string(REGEX MATCH " " F ${${PROJECT_NAME}_BUILD_VERSION})
    if (F) # a valid build number does not contain a space
        set(${PROJECT_NAME}_BUILD_VERSION "unknown")
    endif()
    string(STRIP ${${PROJECT_NAME}_BUILD_VERSION} ${PROJECT_NAME}_BUILD_VERSION)
endif ()

# calculate the tag
set(T_ ${CMAKE_SYSTEM_PROCESSOR})
if (CMAKE_CL_64)
    set(T_ ${T_}_64)
endif (CMAKE_CL_64)
string(TOLOWER ${CMAKE_SYSTEM_NAME} T1_)
set(T_ ${T_}-${T1_})
if (MSVC90)
    set(T1_ "-vc90")
elseif (MSVC10)
    set(T1_ "-vc100")
elseif (MSVC80)
    set(T1_ "-vc80")
elseif (MSVC71)
    set(T1_ "-vc711")
elseif (MSVC70)
    set(T1_ "-vc7")
elseif (MINGW)
    set(T1_ "-mgw")
    exec_program(${CMAKE_CXX_COMPILER}
        ARGS ${CMAKE_CXX_COMPILER_ARG1} -dumpversion
        OUTPUT_VARIABLE T2_
    )
    string(REGEX REPLACE "([0-9])\\.([0-9])(\\.[0-9])?" "\\1\\2" T2_ ${T2_})
    set(T1_ ${T1_}${T2_})
elseif (CMAKE_COMPILER_IS_GNUCXX)
    set(T1_ "-gcc")
    exec_program(${CMAKE_CXX_COMPILER}
        ARGS ${CMAKE_CXX_COMPILER_ARG1} -dumpversion
        OUTPUT_VARIABLE T2_
    )
    string(REGEX REPLACE "([0-9])\\.([0-9])(\\.[0-9])?" "\\1\\2" T2_ ${T2_})
    set(T1_ ${T1_}${T2_})
else()
    set(T1_)
endif()
set(T_ ${T_}${T1_})
set(${PROJECT_NAME}_BUILD_TAG ${T_})
