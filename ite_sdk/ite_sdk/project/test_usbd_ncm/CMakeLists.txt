add_executable(${CMAKE_PROJECT_NAME}
    test_$ENV{CFG_PLATFORM}.c
)

include_directories(
    ${PROJECT_SOURCE_DIR}/sdk/share/ping/
)

if (DEFINED CFG_TEST_PING)
    add_definitions(
        -DCFG_TEST_PING
        -DCFG_PING_IP="${CFG_PING_IP}"
    )
elseif (DEFINED CFG_TEST_NONE)
    add_definitions(
        -DCFG_TEST_NONE
    )
elseif (DEFINED CFG_TEST_SOCKET)
    add_definitions(
        -DCFG_TEST_SOCKET
    )
endif()    

add_executable(${CMAKE_PROJECT_NAME}
    test_usbd_ncm.c
)

# build
include(${PROJECT_SOURCE_DIR}/sdk/build.cmake)
