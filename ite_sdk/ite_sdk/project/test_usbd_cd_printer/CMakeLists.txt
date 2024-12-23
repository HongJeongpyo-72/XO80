include_directories(
    ${PROJECT_SOURCE_DIR}/sdk/driver/usb/inc/
    ${PROJECT_SOURCE_DIR}/sdk/driver/
)

add_executable(${CMAKE_PROJECT_NAME}
    $ENV{CFG_PROJECT}_$ENV{CFG_PLATFORM}.c
)

# build
include(${PROJECT_SOURCE_DIR}/sdk/build.cmake)
