# - Find FTD2XX installation
# This module tries to find the libftd2xx installation on your system.
# Once done this will define
#
#  FTD2XX_FOUND - system has ftd2xx
#  FTD2XX_INCLUDE_DIR - ~ the ftd2xx include directory
#  FTD2XX_LIBRARY - Link these to use ftd2xx
#  FTD2XX_DLL - Path to DLL (windows only)

file(GLOB_RECURSE extern_file ${PROJECT_SOURCE_DIR}/libs/enttec/libenttec/ftd2xx/*ftd2xx.h)
if (extern_file)
    get_filename_component(extern_lib_path ${extern_file} PATH)
    MESSAGE(STATUS "Found FTD2XX library in 'libs/enttec/libenttec/ftd2xx' subfolder: ${extern_lib_path}")
endif(extern_file)

FIND_PATH(FTD2XX_INCLUDE_DIR
        NAMES   ftd2xx.h
        PATHS   /usr/local/include
        /usr/include
        /usr/include/libftd2xx
        /usr/local/include/libftd2xx
        /opt/local/include
        /sw/include
        ${extern_lib_path}
        )
MESSAGE("-- FTD2XX_INCLUDE_DIR: ${FTD2XX_INCLUDE_DIR}")

# determine if we run a 64bit compiler or not
set(bitness i386)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(bitness amd64)
endif()

SET(FTD2XX_LIBNAME ftd2xx)
IF(WIN32)
    SET(FTD2XX_LIBNAME ftd2xx.lib)
ENDIF(WIN32)

FIND_LIBRARY(FTD2XX_LIBRARY
        NAMES ${FTD2XX_LIBNAME}
        PATHS /usr/lib
        /usr/local/lib
        /opt/local/lib
        /sw/lib
        ${extern_lib_path}/${bitness}
        )
MESSAGE("-- FTD2XX_LIBRARY: ${FTD2XX_LIBRARY}")

# set path to DLL for later installation
IF(WIN32 AND FTD2XX_LIBRARY)
    get_filename_component(ftd2xx_lib_path ${FTD2XX_LIBRARY} PATH)
    SET(FTD2XX_DLL ${ftd2xx_lib_path}/ftd2xx.dll)
endif(WIN32 AND FTD2XX_LIBRARY)

IF (FTD2XX_LIBRARY)
    IF(FTD2XX_INCLUDE_DIR)
        SET(FTD2XX_FOUND TRUE)
        MESSAGE(STATUS "Found libFTD2XX: ${FTD2XX_INCLUDE_DIR}, ${FTD2XX_LIBRARY}")
    ELSE(FTD2XX_INCLUDE_DIR)
        SET(FTD2XX_FOUND FALSE)
        MESSAGE(STATUS "libFTD2XX headers NOT FOUND. Make sure to install the development headers! Please refer to the documentation for instructions.")
    ENDIF(FTD2XX_INCLUDE_DIR)
ELSE (FTD2XX_LIBRARY)
    SET(FTD2XX_FOUND FALSE)
    MESSAGE(STATUS "libFTD2XX NOT FOUND.")
ENDIF (FTD2XX_LIBRARY)

SET(FTD2XX_INCLUDE_DIR
    ${FTD2XX_INCLUDE_DIR}
)