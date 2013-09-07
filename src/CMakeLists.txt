##
 # Copyright (c) 2013
 # Massimo Cetra <mcetra at gmail dot com>
 #
 # Permission to use, copy, modify, and distribute this software for any
 # purpose with or without fee is hereby granted, provided that the above
 # copyright notice and this permission notice appear in all copies.
 #
 # THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 # WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 # MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 # ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 # WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 # ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 # OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 ##

SET(SODIUM_LIBRARY_VERSION_MAJOR 4)
SET(SODIUM_LIBRARY_VERSION_MINOR 2)
SET(DLL_VERSION 4)
SET(VERSION 0.${SODIUM_LIBRARY_VERSION_MAJOR}.${SODIUM_LIBRARY_VERSION_MINOR} )

SET(SODIUM_LIBRARY_VERSION=7:0:3)
#                      | | |
#               +------+ | +---+
#               |        |     |
#            current:revision:age
#               |        |     |
#               |        |     +- increment if interfaces have been added
#               |        |        set to zero if interfaces have been removed
#               |        |        or changed
#               |        +- increment if source code has changed
#               |           set to zero if current is incremented
#               +- increment if interfaces have been added, removed or changed


IF(CMAKE_BUILD_TYPE MATCHES debug)
    MESSAGE(STATUS "Enabling DEBUG build")
    SET(ENABLE_DEBUG 1)
ENDIF(CMAKE_BUILD_TYPE MATCHES debug)

# ******************************************************************************
# ************************** MACRO DEFINITION **********************************
# ******************************************************************************

INCLUDE(CheckCCompilerFlag)

MACRO(PROJECT_CHECK_COMPILER_FLAGS)
    SET(CF)

    FOREACH(flag ${ARGN})
        SET(FLAG_NAME   "-${flag}")
        SET(FLAG_OPTION "_${flag}")

        CHECK_C_COMPILER_FLAG("${FLAG_NAME}" ${FLAG_OPTION})
        IF(${FLAG_OPTION})
            SET(CF "${CF} ${FLAG_NAME}")
        #    MESSAGE(STATUS "Enabling compiler flag '${flag}'")
        #ELSE(${FLAG_OPTION})
        #    MESSAGE(STATUS "Not Enabling compiler flag '${flag}'")
        ENDIF(${FLAG_OPTION})

    ENDFOREACH(flag)

    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CF}")
ENDMACRO(PROJECT_CHECK_COMPILER_FLAGS)

# ******************************************************************************
# **************************  ENDIANESS TESTS  *********************************
# ******************************************************************************

INCLUDE(TestBigEndian)

IF(CMAKE_TEST_ENDIANESS_STRINGS_LE)
    ADD_DEFINITIONS("-DNATIVE_LITTLE_ENDIAN=1")
ENDIF(CMAKE_TEST_ENDIANESS_STRINGS_LE)

IF(CMAKE_TEST_ENDIANESS_STRINGS_BE)
    ADD_DEFINITIONS("-DNATIVE_BIG_ENDIAN=1")
ENDIF(CMAKE_TEST_ENDIANESS_STRINGS_BE)

# ******************************************************************************
# *********************** COMPILER FLAGS CHECKS ********************************
# ******************************************************************************

ADD_DEFINITIONS("-D_FORTIFY_SOURCE=2")

PROJECT_CHECK_COMPILER_FLAGS(
    std=c99

    fvisibility=hidden
    fPIC
    fwrapv
    fno-strict-aliasing
    fno-strict-overflow

    Wall
    Wdiv-by-zero
    Wextra
    Winit-self
    Wsometimes-uninitialized
    Wstrict-prototypes
    Wunused
    Wwrite-strings

)

#SET(_LINKER_OPTIONS "-Wl,-z,relro -Wl,-z,now -Wl,-z,noexecstack")

# ******************************************************************************
# ********************* COMMAND LINE FLAGS CHECKS ******************************
# ******************************************************************************

IF(ENABLE_DEBUG)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -g3")
    ADD_DEFINITIONS("-DDEBUG=1")
    PROJECT_CHECK_COMPILER_FLAGS(
        Wno-unknown-warning-option
        Wbad-function-cast
        Wcast-align
        Wcast-qual
        Wchar-subscripts
        Wcomment
        Wfloat-equal
        Wformat=2
        Wimplicit
        Wmissing-declarations
        Wmissing-prototypes
        Wnormalized=id
        Woverride-init
        Wparentheses
        Wpointer-arith
        Wredundant-decls
        Wstrict-prototypes
        Wswitch-enum
        Wvariable-decl
    )
ELSE(ENABLE_DEBUG)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -fomit-frame-pointer -mtune=nocona")
ENDIF(ENABLE_DEBUG)

IF(ENABLE_SSP)
    CHECK_C_COMPILER_FLAG("-fstack-protector" WITH_SSP)
    CHECK_C_COMPILER_FLAG("-fstack-protector-all" WITH_SSP_ALL)
    IF(WITH_SSP_ALL)
        #MESSAGE(STATUS "Enabling stack protector-all")
        SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fstack-protector-all")
    ELSE(WITH_SSP_ALL)
        IF(WITH_SSP)
            #MESSAGE(STATUS "Enabling stack protector")
            SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fstack-protector")
        ENDIF(WITH_SSP)
    ENDIF(WITH_SSP_ALL)
ENDIF(ENABLE_SSP)


IF(ENABLE_PIE)
    CHECK_C_COMPILER_FLAG("-fPIE" WITH_FPIE)
    IF(WITH_FPIE)
        MESSAGE(STATUS "Enabling -fPIE")
        SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIE")
        SET(_LINKER_OPTIONS "-fpie")
    ENDIF(WITH_FPIE)
ENDIF(ENABLE_PIE)


IF(NOT ENABLE_ASM)
    SET(HAVE_AMD64_ASM_V 0)
ELSE(NOT ENABLE_ASM)
    SET(HAVE_AMD64_ASM_V ${ENABLE_ASM})
    # TODO Check HW Support
ENDIF(NOT ENABLE_ASM)


IF(NOT HAVE_TI_MODE)
    SET(HAVE_TI_MODE_V   0)
ELSE(NOT HAVE_TI_MODE)
    SET(HAVE_TI_MODE_V  ${HAVE_TI_MODE})
ENDIF(NOT HAVE_TI_MODE)


IF(ENABLE_BLOCKING_RANDOM)
    ADD_DEFINITIONS("-DUSE_BLOCKING_RANDOM=1")
ENDIF(ENABLE_BLOCKING_RANDOM)


# ******************************************************************************
# ******************************* BUILD PATHS **********************************
# ******************************************************************************

IF(NOT DEFINED CMAKE_SIZEOF_VOID_P)
    MESSAGE(AUTHOR_WARNING
        "Unable to determine default CMAKE_INSTALL_LIBDIR directory because no target architecture is known. "
        "Please enable at least one language before including GNUInstallDirs.")
ENDIF(NOT DEFINED CMAKE_SIZEOF_VOID_P)

IF(NOT LIBSODIUM_DIR)
    SET(LIBSODIUM_DIR /opt/libsodium)
ENDIF(NOT LIBSODIUM_DIR)

IF(NOT LIBSODIUM_INCLUDE_DIR)
    SET(LIBSODIUM_INCLUDE_DIR ${LIBSODIUM_DIR}/include)
ENDIF(NOT LIBSODIUM_INCLUDE_DIR)

IF(NOT LIBSODIUM_LIB_DIR)
    SET(LIBSODIUM_LIB_DIR ${LIBSODIUM_DIR}/lib)

    IF  ("${CMAKE_SIZEOF_VOID_P}" EQUAL "4")
        SET(LIBSODIUM_LIB_DIR ${LIBSODIUM_DIR}/lib32)
    ENDIF  ("${CMAKE_SIZEOF_VOID_P}" EQUAL "4")

    IF("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
        SET(LIBSODIUM_LIB_DIR ${LIBSODIUM_DIR}/lib64)
    ENDIF("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")

ENDIF(NOT LIBSODIUM_LIB_DIR)

MESSAGE(STATUS "---------------------------------------------------" )
MESSAGE(STATUS "LIBSODIUM PREFIX:      " ${LIBSODIUM_DIR} )
MESSAGE(STATUS "LIBSODIUM INCLUDE DIR: " ${LIBSODIUM_INCLUDE_DIR} )
MESSAGE(STATUS "LIBSODIUM LIB DIR:     " ${LIBSODIUM_LIB_DIR} )

# ******************************************************************************
# ******************************************************************************
# ******************************************************************************

ADD_SUBDIRECTORY(libsodium)

