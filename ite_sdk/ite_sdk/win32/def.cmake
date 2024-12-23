# Global compiler options
add_definitions(
    -D_ALLOW_KEYWORD_MACROS
    #-D_CRT_NO_TIME_T
    -D_CRT_SECURE_NO_WARNINGS
    -D_POSIX_MONOTONIC_CLOCK
    -D_POSIX_PRIORITY_SCHEDULING=1
    -D_SCL_SECURE_NO_WARNINGS
    -D_USE_32BIT_TIME_T
    -D_USE_MATH_DEFINES
    -DCURL_STATICLIB
    -DEGLAPI=
    -DEGLAPIENTRY=
    -DLIBXML_STATIC
    -DNOMINMAX
    -DORTP_STATIC
    -DOSIP_MT
    -DPTW32_STATIC_LIB
    -DUPNP_STATIC_LIB
)

# set the startup project for visual studio solution.
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT
    $ENV{CFG_PROJECT}
)

if (CMAKE_BUILD_TYPE STREQUAL Debug)
    add_definitions(
        -D_CRTDBG_MAP_ALLOC
        -DENABLE_TRACE
    )
endif()

ITE_ADD_DEFINITIONS_IF_DEFINED(CFG_WIN32_SIMULATOR)
ITE_ADD_DEFINITIONS_IF_DEFINED(CFG_WIN32_FS_HW)
ITE_ADD_DEFINITIONS_IF_DEFINED_VALUE(CFG_WIN32_LWIP_ADAPTER)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}/$ENV{CFG_PLATFORM}/include
)

# Compiler flags
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

if (DEFINED CFG_WIN32_FS_HW)
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /MP4")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MP4")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_DEBUG}")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_DEBUG}")
else()
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /MTd")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_DEBUG} /MT")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_DEBUG} /MT")
endif()


