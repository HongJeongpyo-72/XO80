add_executable(${CMAKE_PROJECT_NAME}
    test_$ENV{CFG_PLATFORM}.c
    )
	
#=== RTL ===
if (DEFINED CFG_NET_WIFI_SDIO_NGPL OR DEFINED CFG_NET_WIFI_SDIO_NGPL_8723DS)
INCLUDE_DIRECTORIES(
	${PROJECT_SOURCE_DIR}/project/test_wifi_sdio/RTL
)
endif()
    
#=== WIFI SDIO Test ===#
if (DEFINED CFG_TEST_SDIO_WIFI_LWIP)
    add_definitions(
        -DCFG_TEST_SDIO_WIFI_LWIP
        )

if (DEFINED CFG_NET_WIFI_SDIO_NGPL OR DEFINED CFG_NET_WIFI_SDIO_NGPL_8723DS)
    add_executable(${CMAKE_PROJECT_NAME}
		RTL/test_wifi_ngpl_lwip.c
		RTL/iperf_main.c
		RTL/socket_main.c
        RTL/dns_main.c
        RTL/ping_main.c
        RTL/basic_func_main.c
        )
endif()
endif()

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    #=== RTL8189F ===#
if (DEFINED CFG_NET_WIFI_SDIO_NGPL)
	${PROJECT_SOURCE_DIR}/sdk/driver/non_gpl_wifi/include/
    ${PROJECT_SOURCE_DIR}/sdk/driver/non_gpl_wifi/api/wifi/ 
	${PROJECT_SOURCE_DIR}/sdk/share/wifi_mgr/ 
endif()

    #=== RTL8723DS ===#
if (DEFINED CFG_NET_WIFI_SDIO_NGPL_8723DS)
	${PROJECT_SOURCE_DIR}/sdk/driver/non_gpl_wifi_8723d/include/
    ${PROJECT_SOURCE_DIR}/sdk/driver/non_gpl_wifi_8723d/api/wifi/
	${PROJECT_SOURCE_DIR}/sdk/share/wifi_mgr/
endif()

    #=== APP Lib ===#
    ${PROJECT_SOURCE_DIR}/sdk/share/ping/
if (DEFINED CFG_NET_HTTP) 
	${PROJECT_SOURCE_DIR}/sdk/share/iperf/
endif()
    )

# build
include(${PROJECT_SOURCE_DIR}/sdk/build.cmake)
