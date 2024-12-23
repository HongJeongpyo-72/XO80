if (DEFINED CFG_MMC_ENABLE)
else()
	INCLUDE_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}/sdk/driver/sd/mmc/include
	)
endif()


ADD_LIBRARY(mt7682 STATIC
		sdio_hal.c
        sdio_main.c
        hal_sdio_internal.c
        wifi_host_os_api.c
        wifi_stub.c
        wfcm_sdio.c
        
        sdio_host_rw_test.c
    )
