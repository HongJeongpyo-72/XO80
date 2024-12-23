set(PROJ_NAME ${CMAKE_PROJECT_NAME})

if (DEFINED CFG_BOOTLOADER_ENABLE)
    set(CMAKE_PROJECT_NAME bootloader)
    add_subdirectory(../bootloader ../bootloader)
endif()

set(CMAKE_PROJECT_NAME ${PROJ_NAME})

if (DEFINED CFG_NET_WIFI_SDIO_NGPL_AP6256 OR 
    DEFINED CFG_NET_WIFI_SDIO_NGPL_AP6236 OR 
    DEFINED CFG_NET_WIFI_SDIO_NGPL_AP6212)
    SET (WIFI_PATH
        ${PROJECT_SOURCE_DIR}/sdk/driver/non_gpl_wifi_mhd/wlan_lib
    )
endif()

ITE_ADD_DEFINITIONS_IF_DEFINED(CFG_WATCHDOG_ENABLE)

add_definitions(
    -DCFG_WEBSERVER_PORT=${CFG_WEBSERVER_PORT}
    -DCFG_WEBSERVER_PORT_STR="${CFG_WEBSERVER_PORT}"
    -DCFG_HW_VERSION="${CFG_HW_VERSION}"
    -DCFG_UPGRADE_FTP_URL="${CFG_UPGRADE_FTP_URL}"
)

ITE_ADD_DEFINITIONS_IF_DEFINED(CFG_CHECK_FILES_CRC_ON_BOOTING)

if (DEFINED CFG_PLAY_VIDEO_ON_BOOTING)
    add_definitions(
        -DCFG_PLAY_VIDEO_ON_BOOTING
        -DCFG_BOOT_VIDEO_BGCOLOR=${CFG_BOOT_VIDEO_BGCOLOR}
        -DCFG_BOOT_VIDEO_VOLUME=${CFG_BOOT_VIDEO_VOLUME}
    )
endif()

ITE_ADD_DEFINITIONS_IF_DEFINED(CFG_ENABLE_ROTATE)

if (DEFINED CFG_BOOT_VIDEO_ENABLE_WINDOW_MODE)
    add_definitions(
        -DCFG_BOOT_VIDEO_ENABLE_WINDOW_MODE
        -DCFG_VIDEO_WINDOW_X_POS=${CFG_VIDEO_WINDOW_X_POS}
        -DCFG_VIDEO_WINDOW_Y_POS=${CFG_VIDEO_WINDOW_Y_POS}
        -DCFG_VIDEO_WINDOW_WIDTH=${CFG_VIDEO_WINDOW_WIDTH}
        -DCFG_VIDEO_WINDOW_HEIGHT=${CFG_VIDEO_WINDOW_HEIGHT}
    )
endif()

ITE_ADD_DEFINITIONS_IF_DEFINED(CFG_SCREENSHOT_ENABLE)

if (DEFINED CFG_UPGRADE_GUI)
    add_definitions(
        -DCFG_UPGRADE_GUI
    )
    execute_process(COMMAND dataconv
        -x ${PROJECT_SOURCE_DIR}/project/${CMAKE_PROJECT_NAME}/${CFG_ITU_PATH}/upgrade.itu
        -o ${CMAKE_CURRENT_BINARY_DIR}/upgrade.inc
    )        
endif()

if (DEFINED CFG_DYNAMIC_LOAD_TP_MODULE)
    add_definitions(
        -DCFG_DYNAMIC_LOAD_TP_MODULE
    )      
        
    if (DEFINED CFG_TOUCH_ENABLE)

        unset(modules)
        foreach (m ${CFG_TOUCH_MODULE})
            set(modules "${modules} ${m}")
        endforeach()
    
        add_definitions(
            -DCFG_CTRLBOARD_TOUCH_MODULE="${modules}"
        ) 
    endif()
endif()

ITE_ADD_DEFINITIONS_IF_DEFINED(CFG_AUTO_RUN)
ITE_ADD_DEFINITIONS_IF_DEFINED(CFG_AUTO_CHANGE_TYPE)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    if (DEFINED CFG_NET_WIFI_SDIO_NGPL)
        ${PROJECT_SOURCE_DIR}/sdk/driver/non_gpl_wifi/include/
        ${PROJECT_SOURCE_DIR}/sdk/driver/non_gpl_wifi/api/wifi/
    endif()
    if (DEFINED CFG_NET_WIFI_SDIO_NGPL_8723DS)
        ${PROJECT_SOURCE_DIR}/sdk/driver/non_gpl_wifi_8723d/include/
        ${PROJECT_SOURCE_DIR}/sdk/driver/non_gpl_wifi_8723d/api/wifi/
    endif()
)

if (DEFINED CFG_NET_WIFI_SDIO_NGPL_AP6256 OR 
    DEFINED CFG_NET_WIFI_SDIO_NGPL_AP6236 OR 
    DEFINED CFG_NET_WIFI_SDIO_NGPL_AP6212 OR 
    DEFINED CFG_NET_WIFI_SDIO_NGPL_AP6203)	
    include_directories(
        ${CMAKE_CURRENT_BINARY_DIR}
        ${PROJECT_SOURCE_DIR}/sdk/driver/non_gpl_wifi_mhd/include	
    )
endif()

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}/sdk/driver/mmp/include
	${PROJECT_SOURCE_DIR}/sdk/include
    ${PROJECT_SOURCE_DIR}/sdk/share/audio_mgr
    ${PROJECT_SOURCE_DIR}/sdk/share/ffmpeg
    ${PROJECT_SOURCE_DIR}/sdk/share/lwip_ftpd
    ${PROJECT_SOURCE_DIR}/sdk/share/microhttpd
    ${PROJECT_SOURCE_DIR}/sdk/share/microhttpd/include
    ${PROJECT_SOURCE_DIR}/sdk/share/wifi_mgr/ 
    ${PROJECT_SOURCE_DIR}/sdk/driver/itv
    ${PROJECT_SOURCE_DIR}/sdk/share/qrencode

    if (CFG_CHIP_FAMILY STREQUAL 9070)
        include_directories(
            ${PROJECT_SOURCE_DIR}/sdk/share/itu
            ${PROJECT_SOURCE_DIR}/sdk/share/itu/it9070
        )
    else()
        include_directories(
            ${PROJECT_SOURCE_DIR}/sdk/share/itu
            ${PROJECT_SOURCE_DIR}/sdk/share/itu/it9850
        )
    endif()
)
    
### AMPAK ###
if (DEFINED CFG_BTA_ENABLE)
    add_definitions(
        -DCFG_BTA_ENABLE
    )

    SET (RTOS openrtos)
    SET (PLATFORM_VENDOR ite)

    SET (BTA_PATH
        ${PROJECT_SOURCE_DIR}/sdk/share/bluetooth_bta/BTA_Release
    )

    set (BRCM_BTAPP_DEMO_SRC
        ${BTA_PATH}/Projects/bta/btapp_src/btapp.c
        ${BTA_PATH}/Projects/bta/btapp_src/btapp_avk.c
        ${BTA_PATH}/Projects/bta/btapp_src/btapp_dg.c
        ${BTA_PATH}/Projects/bta/btapp_src/btapp_dm.c
        ${BTA_PATH}/Projects/bta/btapp_src/btapp_hs.c
        ${BTA_PATH}/Projects/bta/btapp_src/btapp_pan.c
        ${BTA_PATH}/Projects/bta/btapp_src/btapp_rc.c
        ${BTA_PATH}/Projects/bta/bleapp_src/btapp_gattc.c
        ${BTA_PATH}/Projects/bta/bleapp_src/btapp_gattc_profile.c
        ${BTA_PATH}/Projects/bta/bleapp_src/btapp_ble_main.c
    )

    set (BRCM_BTAPP_COMM_SRC
        ${BTA_PATH}/Projects/bta/btapp_comm_src/btapp_cfg.c
        ${BTA_PATH}/Projects/bta/btapp_comm_src/btapp_codec_asnk.c
        ${BTA_PATH}/Projects/bta/btapp_comm_src/btapp_nv.c
        ${BTA_PATH}/Projects/bta/btapp_comm_src/btapp_utility.c
        ${BTA_PATH}/Projects/bta/btapp_comm_src/tnt_ble_nus.c
    )


    SET (BTA_COMMON
        ${BTA_PATH}/../port_${PLATFORM_VENDOR}/src/upio_ite.c
        ${BTA_PATH}/../port_${PLATFORM_VENDOR}/src/userial_ite.c
        ${BTA_PATH}/../port_${PLATFORM_VENDOR}/src/audio_ite.c
        ${BTA_PATH}/../port_${PLATFORM_VENDOR}/src/ucodec_ite.c
        ${BTA_PATH}/../port_${PLATFORM_VENDOR}/src/audio_ctrl.c
        ${BTA_PATH}/Projects/bte/main/bte_disp_stubs.c    
        ${BTA_PATH}/Projects/bte/main/bte_hcisu.c    
        ${BTA_PATH}/Projects/bte/main/bte_init.c    
        ${BTA_PATH}/Projects/bte/main/bte_load.c    
        ${BTA_PATH}/Projects/bte/main/bte_logmsg.c    
        ${BTA_PATH}/Projects/bte/main/bte_main.c    
        ${BTA_PATH}/Projects/bte/main/bte_post_reset.c
    )

    SET (BTEINCLUDES
        ${BTA_PATH}/
        ${BTA_PATH}/Components/bta/ag
        ${BTA_PATH}/Components/bta/ar
        ${BTA_PATH}/Components/bta/avk
        ${BTA_PATH}/Components/bta/brcm
        ${BTA_PATH}/Components/bta/dg
        ${BTA_PATH}/Components/bta/dm
        ${BTA_PATH}/Components/bta/gatt
        ${BTA_PATH}/Components/bta/hd
        ${BTA_PATH}/Components/bta/hs
        ${BTA_PATH}/Components/bta/include
        ${BTA_PATH}/Components/bta/rc
        ${BTA_PATH}/Components/bta/sys
        ${BTA_PATH}/Components/gki/common
        ${BTA_PATH}/Components/gki/${RTOS}/
        ${BTA_PATH}/Components/hcis
        ${BTA_PATH}/Components/hcis/include
        ${BTA_PATH}/Components/stack/include
        ${BTA_PATH}/Components/stack/avrc
        ${BTA_PATH}/Components/stack/avct
        ${BTA_PATH}/Components/stack/avdt
        ${BTA_PATH}/Components/stack/a2dp
        ${BTA_PATH}/Components/stack/rfcomm
        ${BTA_PATH}/Components/stack/btm
        ${BTA_PATH}/Components/stack/brcm
        ${BTA_PATH}/Components/stack/l2cap
        ${BTA_PATH}/Components/stack/sdp
        ${BTA_PATH}/Components/stack/gap
        ${BTA_PATH}/Components/stack/gatt
        ${BTA_PATH}/Components/stack/smp
        ${BTA_PATH}/Components/stack/hid
        ${BTA_PATH}/Components/stack/hsp2
        ${BTA_PATH}/Components/stack/srvc
        ${BTA_PATH}/Components/rpc/include
        ${BTA_PATH}/Components/udrv/include
        ${BTA_PATH}/Components/stack/include
        ${BTA_PATH}/Components/embdrv/sbc/decoder/include
        ${BTA_PATH}/Components/embdrv/sbc/encoder/include
        ${BTA_PATH}/Projects/bte/main
        ${BTA_PATH}/Projects/bta/bleapp_inc
        ${BTA_PATH}/Projects/bta/btapp_comm_inc
        ${BTA_PATH}/Projects/bta/btapp_inc
        ${BTA_PATH}/Projects/bta/bleapp_inc
        ${BTA_PATH}/Projects/bta/bta_co
        ${BTA_PATH}/Projects/proto_disp
        ${BTA_PATH}/../port_${PLATFORM_VENDOR}/inc/
    )

    include_directories(
        .
        ${BTEINCLUDES}
    )

    if (DEFINED CFG_NET_WIFI_SDIO_NGPL_AP6256)
        SET (BT_MODULE AP6256)
    endif()

    if (DEFINED CFG_NET_WIFI_SDIO_NGPL_AP6236)
        SET (BT_MODULE AP6236)
    endif()

    if (DEFINED CFG_NET_WIFI_SDIO_NGPL_AP6203)
        SET (BT_MODULE AP6203)
    endif()

    set (BRCM_BT_HCD_SRC
        ${BTA_PATH}/Projects/bte/fwpatches/${BT_MODULE}/bt_firmware_image.c
    )

    add_executable(${CMAKE_PROJECT_NAME}
        audio.c
        config.c
        ctrlboard.h
        external.c
        external_process.c
        function_table.c
        main.c
        photo.c
        reset.c
        scene.c
        scene.h
        screen.c
        storage.c
        string.c
        upgrade.c
        backup.c
        network_func.c
        network_main.c
        network_config.h
        network_wifi.c
        nework_mirror.c
        ${BRCM_BTAPP_DEMO_SRC}
        ${BRCM_BTAPP_COMM_SRC}
        ${BTA_COMMON}
        ${BRCM_BT_HCD_SRC}
        btapp_gatts.c
    )
    set (PREBUILD_LIB 
        ${BTA_PATH}/libs/libbluetooth_bta.a
    )
    file(COPY ${PREBUILD_LIB}
        DESTINATION ${CMAKE_BINARY_DIR}/lib/${CFG_CPU_NAME}/
    )

    target_link_libraries(${CMAKE_PROJECT_NAME}
        bluetooth_bta
    )
else()
    add_executable(${CMAKE_PROJECT_NAME}
        audio.c
        config.c
        ctrlboard.h
        external.c
        external_process.c
        function_table.c
        main.c
        photo.c
        reset.c
        scene.c
        scene.h
        screen.c
        storage.c
        string.c
        upgrade.c
        backup.c
		video_file_process.c
    )
    if (DEFINED CFG_NET_ENABLE)	
        add_executable(${CMAKE_PROJECT_NAME}
            network_func.c
            network_main.c
            network_config.h
            network_wifi.c
            nework_mirror.c
        )
    endif()
endif()
### AMPAK ###

# layers
include(layer.cmake)

#if (DEFINED CFG_NET_ENABLE)
#    add_executable(${CMAKE_PROJECT_NAME}
#        webserver.c
#    )
#endif()

# boot video
if (DEFINED CFG_PLAY_VIDEO_ON_BOOTING)
    add_executable(${CMAKE_PROJECT_NAME}
        video.c
    )
        
    file(COPY
        ${PROJECT_SOURCE_DIR}/project/${CMAKE_PROJECT_NAME}/media/boot/boot.mp4
        DESTINATION ${CMAKE_BINARY_DIR}/data/private/media
    )
endif()

if (DEFINED CFG_PLAY_MJPEG_ON_BOOTING)
    add_executable(${CMAKE_PROJECT_NAME}
        video_mjpeg.c
    )
        
    file(COPY
        ${PROJECT_SOURCE_DIR}/project/${CMAKE_PROJECT_NAME}/media/boot/${CFG_BOOT_MJPEG_FILE}
        DESTINATION ${CMAKE_BINARY_DIR}/data/private/media
    )
endif()

# files
configure_file(${PROJECT_SOURCE_DIR}/project/${CMAKE_PROJECT_NAME}/ctrlboard.ini ${CMAKE_BINARY_DIR}/data/public/ctrlboard.ini COPYONLY)
file(COPY
    ${PROJECT_SOURCE_DIR}/project/${CMAKE_PROJECT_NAME}/sounds
    DESTINATION ${CMAKE_BINARY_DIR}/data/private
)

if (DEFINED CFG_LCD_MULTIPLE)
    file(COPY
        ${PROJECT_SOURCE_DIR}/project/${CMAKE_PROJECT_NAME}/itu
        DESTINATION ${CMAKE_BINARY_DIR}/data/private
        PATTERN "*.xml" EXCLUDE
        PATTERN "*.bak" EXCLUDE
        PATTERN "*.xls" EXCLUDE
        PATTERN "*_rotate" EXCLUDE
        PATTERN "upgrade.itu" EXCLUDE
    )
else()
    file(GLOB files ${PROJECT_SOURCE_DIR}/project/${CMAKE_PROJECT_NAME}/${CFG_ITU_PATH}/*.*)
    file(COPY
        ${files}
        DESTINATION ${CMAKE_BINARY_DIR}/data/private
        PATTERN "*.xml" EXCLUDE
        PATTERN "*.bak" EXCLUDE    
    )
endif()

if (DEFINED CFG_NET_WIFI_WPA_ENABLE)
    configure_file(
        ${PROJECT_SOURCE_DIR}/sdk/share/wifi_hostapd/hostapd_wpa.conf.in
        ${CMAKE_BINARY_DIR}/data/public/hostapd.conf
        NEWLINE_STYLE UNIX
    )
else()
    configure_file(
        ${PROJECT_SOURCE_DIR}/sdk/share/wifi_hostapd/hostapd.conf.in
        ${CMAKE_BINARY_DIR}/data/public/hostapd.conf
        NEWLINE_STYLE UNIX
    )
endif()

# backups
configure_file(${PROJECT_SOURCE_DIR}/project/${CMAKE_PROJECT_NAME}/ctrlboard.ini ${CMAKE_BINARY_DIR}/data/private/backup/${CFG_PUBLIC_DRIVE}/ctrlboard.ini COPYONLY)
# crc
if (DEFINED CFG_CHECK_FILES_CRC_ON_BOOTING)
    file(REMOVE ${CMAKE_BINARY_DIR}/data/private/backup/${CFG_PUBLIC_DRIVE}/ite_crc.dat)
    execute_process(COMMAND makecrc -c ${CMAKE_BINARY_DIR}/data/public/ite_crc.dat -i ${CMAKE_BINARY_DIR}/data/private/backup/${CFG_PUBLIC_DRIVE})
    execute_process(COMMAND makecrc -v ${CMAKE_BINARY_DIR}/data/public/ite_crc.dat -i ${CMAKE_BINARY_DIR}/data/private/backup/${CFG_PUBLIC_DRIVE})
    file(COPY ${CMAKE_BINARY_DIR}/data/public/ite_crc.dat DESTINATION ${CMAKE_BINARY_DIR}/data/private/backup/${CFG_PUBLIC_DRIVE})
endif()

# font
file(COPY
    ${PROJECT_SOURCE_DIR}/project/${CMAKE_PROJECT_NAME}/font/${CFG_FONT_FILENAME}
    DESTINATION ${CMAKE_BINARY_DIR}/data/private/font
    )
	
# font 1
file(COPY
    ${PROJECT_SOURCE_DIR}/project/${CMAKE_PROJECT_NAME}/font/WenQuanYiMicroHeiMono.ttf
    DESTINATION ${CMAKE_BINARY_DIR}/data/private/font
)


# web
file(GLOB files ${PROJECT_SOURCE_DIR}/project/${CMAKE_PROJECT_NAME}/web/*.*)
foreach (src ${files})
    string(REPLACE "${PROJECT_SOURCE_DIR}/project/${CMAKE_PROJECT_NAME}/web/" "${CMAKE_BINARY_DIR}/data/private/web/" dest ${src})
    configure_file(${src} ${dest} COPYONLY)
endforeach()

# screen saver
file(COPY
    ${PROJECT_SOURCE_DIR}/project/${CMAKE_PROJECT_NAME}/screensaver.jpg
    DESTINATION ${CMAKE_BINARY_DIR}/data/public
)

# codec
if (DEFINED CFG_COMPRESS_AUDIO_PLUGIN)
    configure_file(${PROJECT_SOURCE_DIR}/build/$ENV{CFG_BUILDPLATFORM}/codec/lib/sm32/mp3.codecs ${CMAKE_BINARY_DIR}/data/private/codec/mp3.codecs COPYONLY)
    configure_file(${PROJECT_SOURCE_DIR}/build/$ENV{CFG_BUILDPLATFORM}/codec/lib/sm32/aac.codecs ${CMAKE_BINARY_DIR}/data/private/codec/aac.codecs COPYONLY)
    configure_file(${PROJECT_SOURCE_DIR}/build/$ENV{CFG_BUILDPLATFORM}/codec/lib/sm32/wma.codecs ${CMAKE_BINARY_DIR}/data/private/codec/wma.codecs COPYONLY)
    configure_file(${PROJECT_SOURCE_DIR}/build/$ENV{CFG_BUILDPLATFORM}/codec/lib/sm32/wave.codecs ${CMAKE_BINARY_DIR}/data/private/codec/wave.codecs COPYONLY)
endif()

if (DEFINED CFG_NET_WIFI_WPA_ENABLE)
    configure_file(
        ${PROJECT_SOURCE_DIR}/sdk/share/wifi_hostapd/hostapd_wpa.conf.in
        ${CMAKE_BINARY_DIR}/data/private/backup/${CFG_PUBLIC_DRIVE}/hostapd.conf
        NEWLINE_STYLE UNIX
    )
else()    
    configure_file(
        ${PROJECT_SOURCE_DIR}/sdk/share/wifi_hostapd/hostapd.conf.in
        ${CMAKE_BINARY_DIR}/data/private/backup/${CFG_PUBLIC_DRIVE}/hostapd.conf
        NEWLINE_STYLE UNIX
    )
endif()

# media
file(GLOB files ${PROJECT_SOURCE_DIR}/project/${CMAKE_PROJECT_NAME}/media/*.*)
foreach (src ${files})
   string(REPLACE "${PROJECT_SOURCE_DIR}/project/${CMAKE_PROJECT_NAME}/media/" "${CMAKE_BINARY_DIR}/data/public/media/" dest ${src})
   configure_file(${src} ${dest} COPYONLY)
endforeach()

if (DEFINED CFG_DYNAMIC_LOAD_TP_MODULE)
    add_executable(${CMAKE_PROJECT_NAME}
        load_tp_module.c 
    )
endif()

if (DEFINED CFG_NET_WIFI_SDIO_NGPL_AP6256)
    if (DEFINED CFG_NET_LWIP)
        set (PREBUILD_LIB 
            ${WIFI_PATH}/libmhd4345x-LwIP_1_4_1.a
        )	
    endif()
    if (DEFINED CFG_NET_LWIP_2)
        set (PREBUILD_LIB 
            ${WIFI_PATH}/libmhd4345x-LwIP_2_1_2.a
        )
    endif()		
endif()

if (DEFINED CFG_NET_WIFI_SDIO_NGPL_AP6236)
    if (DEFINED CFG_NET_LWIP)
        set (PREBUILD_LIB 
            ${WIFI_PATH}/libmhd4343x-LwIP_1_4_1.a
        )	
    endif()
    if (DEFINED CFG_NET_LWIP_2)
        set (PREBUILD_LIB 
            ${WIFI_PATH}/libmhd4343x-LwIP_2_1_2.a
        )
    endif()		
endif()

if (DEFINED CFG_NET_WIFI_SDIO_NGPL_AP6212)
    if (DEFINED CFG_NET_LWIP)
        set (PREBUILD_LIB 
            ${WIFI_PATH}/libmhd43438-LwIP_1_4_1.a
        )	
    endif()
    if (DEFINED CFG_NET_LWIP_2)
        set (PREBUILD_LIB 
            ${WIFI_PATH}/libmhd43438-LwIP_2_1_2.a
        )
    endif()		
endif()

if (DEFINED CFG_NET_WIFI_SDIO_NGPL_AP6203)
    if (DEFINED CFG_NET_LWIP)
        set (PREBUILD_LIB 
            ${WIFI_PATH}/libmhd4301x-LwIP_1_4_1.a
        )	
    endif()
    if (DEFINED CFG_NET_LWIP_2)
        set (PREBUILD_LIB 
            ${WIFI_PATH}/libmhd4301x-LwIP_2_1_2.a
        )
    endif()		
endif()

if (DEFINED CFG_NET_WIFI_SDIO_NGPL_AP6256 OR 
    DEFINED CFG_NET_WIFI_SDIO_NGPL_AP6236 OR 
    DEFINED CFG_NET_WIFI_SDIO_NGPL_AP6212)
    file(COPY ${PREBUILD_LIB}
    DESTINATION ${CMAKE_BINARY_DIR}/lib/${CFG_CPU_NAME}/
)
endif()

# build
include(${PROJECT_SOURCE_DIR}/sdk/build.cmake)
