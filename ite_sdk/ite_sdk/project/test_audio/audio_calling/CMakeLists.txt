add_executable(${CMAKE_PROJECT_NAME}
    test_$ENV{CFG_PLATFORM}.c
    )

if(DEFINED CFG_AUDIO_ENABLE)
    include_directories(
        ${PROJECT_SOURCE_DIR}/sdk/include/flower
        )
    if(DEFINED CFG_AUDIOFILTER_CALLING_TEST)
        add_executable(${CMAKE_PROJECT_NAME}
            audio_calling/test_leaf_calling.c
            audio_calling/network.c
        )
    elseif(DEFINED CFG_AUDIOFILTER_CALLED_TEST)
        add_executable(${CMAKE_PROJECT_NAME}
            audio_called/test_leaf_called.c
            audio_called/network.c
        )  
    endif()    
endif()
# build
include(${PROJECT_SOURCE_DIR}/sdk/build.cmake)
