if (DEFINED CFG_GCC_LTO)
    add_definitions("-fno-lto")
endif ()

include_directories(
	${PROJECT_SOURCE_DIR}/sdk/share/
    ${PROJECT_SOURCE_DIR}/sdk/share/flower/
    ${PROJECT_SOURCE_DIR}/sdk/include/
    ${PROJECT_SOURCE_DIR}/sdk/share/ffmpeg
    ${PROJECT_SOURCE_DIR}/openrtos/include/
    if(DEFINED CFG_AUDIO_ENABLE)
    ${PROJECT_SOURCE_DIR}/sdk/share/audio_preprocess
    endif()
    if(DEFINED CFG_BUILD_AUDIO_MGR)
    ${PROJECT_SOURCE_DIR}/sdk/share/audiomgr/
    endif()
)

add_library(flower STATIC
    filter_chain.c
    example/filterA.c
    example/filterB.c
    example/filterC.c
    example/filterD.c
    example/filterE.c
    example/filterF.c
    filter_des.c
    ite_queue.c
    ite_buffer.c
    flower_stream.c
)

if(DEFINED CFG_NET_ENABLE)
add_library(flower STATIC
    filter_udp.c
    filter_tcp.c
	)
endif()

if (DEFINED CFG_VIDEO_ENABLE)
add_library(flower STATIC
	video/filter_capture.c
	video/filter_isp.c
	video/filter_h264enc.c
	video/filter_h264dec.c
	video/filter_display.c
	video/filter_displayosd.c
	video/filter_jpegwriter.c
	video/filter_filewriter.c
	video/filter_rec_avi.c
	video/ite_avienc.h
	video/ite_avienc.c
	video/filter_ipcam.h
	video/filter_ipcam.c
	)
endif()

if(DEFINED CFG_QR_DECODE_ENABLE)
add_library(flower STATIC
	video/filter_qrscanner.c
)
endif()

if(DEFINED CFG_AUDIO_ENABLE)
    add_library(flower STATIC
    audio/audiofilterflow.c
    audio/audioqueue.c
    audio/volumeapi.c
    audio/Ffileplay.c
    audio/Ffilerec.c
    audio/Fcastor3snd.c
    audio/Ffilemix.c
    audio/Fulaw.c
    audio/Falaw.c
    audio/Fcmerge.c
    audio/Fcseparate.c
	audio/Fvoid.c
	audio/Ftee.c
    audio/Fvolume.c
    audio/Fhalfduplex.c
    audio/Fresample.c
    audio/Fchanadapt.c
	audio/Fmix.c
 	audio/Floopback.c
    )
endif()

if(DEFINED CFG_BUILD_AUDIO_PREPROCESS)
    add_library(flower STATIC
    audio/Faec.c
    audio/Fdenoise.c
    )
endif()

if(DEFINED CFG_BUILD_ASR)
    add_library(flower STATIC
    audio/asrfilterflow.c
    audio/Fasr.c
    )
endif()

if(DEFINED CFG_BUILD_AUDIO_MGR)
    add_library(flower STATIC
    audio/Ffileplaymgr.c
    audio/Fstreammgr.c
    )
endif()