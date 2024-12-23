add_executable(${CMAKE_PROJECT_NAME}
    test_$ENV{CFG_PLATFORM}.c
)

add_executable(${CMAKE_PROJECT_NAME}
    test_main.c
    example.c
    color_conv.c
    color_conv.h
    jpg.c
    jpg.h
    packet_queue.c
    packet_queue.h
)

# build
include(${PROJECT_SOURCE_DIR}/sdk/build.cmake)
