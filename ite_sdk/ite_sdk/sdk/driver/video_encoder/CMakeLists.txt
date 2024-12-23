if (CFG_CHIP_FAMILY STREQUAL 970)
    include_directories(
        ${PROJECT_SOURCE_DIR}/sdk/driver/h264_encoder/inc
        ${PROJECT_SOURCE_DIR}/sdk/driver/h264_encoder/h264
        ${PROJECT_SOURCE_DIR}/sdk/driver/h264_encoder/common
        ${PROJECT_SOURCE_DIR}/sdk/driver/h264_encoder/camstab
        ${PROJECT_SOURCE_DIR}/sdk/include/capture
        ${PROJECT_SOURCE_DIR}/sdk/include/sensor
    )

if (DEFINED CFG_IPTV_TX)
    ADD_LIBRARY(video_encoder STATIC
        video_encoder_iptv.c
    )
elseif(DEFINED CFG_BUILD_FLOWER)
    ADD_LIBRARY(video_encoder STATIC
         video_encoder_flower.c
    )
else()
    ADD_LIBRARY(video_encoder STATIC
        video_encoder_it970.c
    )
endif()

    TARGET_LINK_LIBRARIES(video_encoder
        h264_encoder
        sensor
        capture
        isp
    )
else()
    ADD_LIBRARY(video_encoder STATIC
        video_encoder.c
    )
endif()
