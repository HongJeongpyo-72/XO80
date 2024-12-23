add_executable(${CMAKE_PROJECT_NAME}
    test_$ENV{CFG_PLATFORM}.c
    ) 
    
if(DEFINED CFG_TEST_HW_PLAY)
    add_executable(${CMAKE_PROJECT_NAME}
        #audioHWctrl/audio_ctrl.c
        audioHWctrl/test_main_simple.c
    )
elseif(DEFINED CFG_TEST_HW_USBWAV_PLAY)
    add_executable(${CMAKE_PROJECT_NAME}
        #audioHWctrl/audio_ctrl.c
        audioHWctrl/gwavhead.c
        audioHWctrl/test_main_usb_play.c
    )
elseif(DEFINED CFG_TEST_HW_USBWAV_RECORD)
    add_executable(${CMAKE_PROJECT_NAME}
        #audioHWctrl/audio_ctrl.c
        audioHWctrl/gwavhead.c
        audioHWctrl/test_main_usb_record.c
    )
#elseif(DEFINED CFG_AD_REC_DA_PLAY_TEST)
#    add_executable(${CMAKE_PROJECT_NAME}
#        audioHWctrl/audio_ctrl.c
#        audioHWctrl/test_main_adc_rec_play.c
#    )
elseif(DEFINED CFG_TEST_HW_LOUDER)
    add_executable(${CMAKE_PROJECT_NAME}
        #audioHWctrl/audio_ctrl.c
        audioHWctrl/test_main_louder.c
    )
elseif(DEFINED CFG_TEST_AUDIOMGR_PLAY)
    include_directories(${PROJECT_SOURCE_DIR}/sdk/share/audio_mgr)
    add_executable(${CMAKE_PROJECT_NAME}
        audioMgr/test_main_audiomgr.c
        audioMgr/audio.c
        audioMgr/sdl/main_processor_message_queue.c
    )
elseif(DEFINED CFG_MP3_CODEC_TEST)
    include_directories(${PROJECT_SOURCE_DIR}/sdk/share/audio_mgr)
    add_executable(${CMAKE_PROJECT_NAME}
        audioMgr/test_mp3codec.c
        audioMgr/audio.c
        audioMgr/sdl/main_processor_message_queue.c
    )  
elseif (DEFINED CFG_TEST_FLOWER_PLAY)
    include_directories(${PROJECT_SOURCE_DIR}/sdk/include/flower)
    include_directories(${PROJECT_SOURCE_DIR}/sdk/share/audio_mgr)
    add_executable(${CMAKE_PROJECT_NAME}
        audioFlow/test_flower_play.c
        audioFlow/sdl/audio.c
        audioFlow/sdl/main_processor_message_queue.c
    )
elseif (DEFINED CFG_TEST_FLOWER_USBREC)
    include_directories(${PROJECT_SOURCE_DIR}/sdk/include/flower)
    add_executable(${CMAKE_PROJECT_NAME}
        audioFlow/test_flower_usbrec.c
    )
elseif (DEFINED CFG_TEST_FLOWER_MIX)
    include_directories(${PROJECT_SOURCE_DIR}/sdk/include/flower)
	#include_directories(${PROJECT_SOURCE_DIR}/sdk/share/audio_mgr)
    add_executable(${CMAKE_PROJECT_NAME}
        audioFlow/test_flower_mix.c
		#audioFlow/sdl/audio.c
		#audioFlow/sdl/main_processor_message_queue.c
    )
elseif (DEFINED CFG_TEST_FLOWER_ASR)
    include_directories(${PROJECT_SOURCE_DIR}/sdk/include/flower)
    add_executable(${CMAKE_PROJECT_NAME}
        audioFlow/test_flower_asr.c
    )
elseif (DEFINED CFG_TEST_FLOWER_LAOUDER)
    include_directories(${PROJECT_SOURCE_DIR}/sdk/include/flower)
    add_executable(${CMAKE_PROJECT_NAME}
        audioFlow/test_flower_louder.c
    )
elseif (DEFINED CFG_TEST_FLOWER_LAOUDER_MIX)
    include_directories(${PROJECT_SOURCE_DIR}/sdk/include/flower)
    add_executable(${CMAKE_PROJECT_NAME}
        audioFlow/test_flower_louder_mix.c
    )
elseif (DEFINED CFG_TEST_FLOWER_ANALOGSPK)
    include_directories(${PROJECT_SOURCE_DIR}/sdk/include/flower)
    add_executable(${CMAKE_PROJECT_NAME}
        audioFlow/test_flower_analogspk.c
    )
elseif (DEFINED CFG_TEST_FLOWER_RESAMPLE_PLAY)
    include_directories(${PROJECT_SOURCE_DIR}/sdk/include/flower)
    include_directories(${PROJECT_SOURCE_DIR}/sdk/share/audio_mgr)
    add_executable(${CMAKE_PROJECT_NAME}
        audioFlow/test_flower_resample_play.c
        audioFlow/sdl/audio.c
        audioFlow/sdl/main_processor_message_queue.c
    )
elseif (DEFINED CFG_TEST_FLOWER_RESAMPLE_PLAY_ASR)
    include_directories(${PROJECT_SOURCE_DIR}/sdk/include/flower)
	include_directories(${PROJECT_SOURCE_DIR}/sdk/share/audio_mgr)
    add_executable(${CMAKE_PROJECT_NAME}
        audioFlow/test_flower_resample_play_with_asr.c
		audioFlow/sdl/audio.c
		audioFlow/sdl/main_processor_message_queue.c
    )
elseif (DEFINED CFG_TEST_FLOWER_STREAM_PLAY)
    include_directories(${PROJECT_SOURCE_DIR}/sdk/include/flower)
    include_directories(${PROJECT_SOURCE_DIR}/sdk/share/audio_mgr)
    add_executable(${CMAKE_PROJECT_NAME}
        audioFlow/test_flower_stream.c
        audioFlow/sdl/audio.c
        audioFlow/sdl/main_processor_message_queue.c
    )
#elseif(DEFINED CFG_FFT_SNR_TEST)
#    add_executable(${CMAKE_PROJECT_NAME}
#        fft/fft_tool.c
#        audioHWctrl/gwavhead.c
#        audioHWctrl/test_main_fft_snr.c
#    )
    
#elseif(DEFINED CFG_BIST_OUT_RECORD_TEST)
#    add_executable(${CMAKE_PROJECT_NAME}
#        audioHWctrl/gwavhead.c
#        audioHWctrl/test_main_bist_record.c
#    )

endif()

# build
include(${PROJECT_SOURCE_DIR}/sdk/build.cmake)
