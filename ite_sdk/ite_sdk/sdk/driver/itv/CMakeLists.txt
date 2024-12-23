INCLUDE_DIRECTORIES(
    .
    ${PROJECT_SOURCE_DIR}/sdk/include/
    ${PROJECT_SOURCE_DIR}/sdk/include/ite/
    ${PROJECT_SOURCE_DIR}/sdk/share/ffmpeg/
    #${PROJECT_SOURCE_DIR}/sdk/share/itu/it9850
)

LINK_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}/sdk/share/ffmpeg
)

if (CFG_CHIP_FAMILY STREQUAL 970 OR CFG_CHIP_FAMILY STREQUAL 960)
    if (DEFINED CFG_IPTV_RX)
        add_library(itv STATIC
            ${PROJECT_SOURCE_DIR}/sdk/include/ite/itv.h
            it970/itv_iptv.c
            fc_sync.h
            fc_sync.c
        )
    else()
        add_library(itv STATIC
            ${PROJECT_SOURCE_DIR}/sdk/include/ite/itv.h
            it970/itv.c
            fc_sync.h
            fc_sync.c
        )
    endif()
endif()

target_link_libraries(itv
    isp
)
