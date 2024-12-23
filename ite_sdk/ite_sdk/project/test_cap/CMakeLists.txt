include_directories(
    ${PROJECT_SOURCE_DIR}/sdk/include/
    ${PROJECT_SOURCE_DIR}/sdk/include/capture/capture_970
	${PROJECT_SOURCE_DIR}/sdk/share/ffmpeg
)
add_executable(${CMAKE_PROJECT_NAME}
    test_$ENV{CFG_PLATFORM}.c
    )
if (DEFINED CFG_CAP_MODULE_MODE)
add_executable(${CMAKE_PROJECT_NAME}
    test_modules.c
    )
elseif(DEFINED CFG_CAP_HDMIRX_MODE)
add_executable(${CMAKE_PROJECT_NAME}
    test_hdmirx.c
    test_itv.c
    )
else()
add_executable(${CMAKE_PROJECT_NAME}
    test_delaywindows.c
    )
endif()

if(DEFINED CFG_I2S_ENABLE AND CFG_AUDIO_ENABLE)
add_executable(${CMAKE_PROJECT_NAME}
    audio_ctrl.c
    gwavhead.c
)
endif()

target_link_libraries(${CMAKE_PROJECT_NAME}
)
# build
include(${PROJECT_SOURCE_DIR}/sdk/build.cmake)
