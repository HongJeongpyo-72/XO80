INCLUDE_DIRECTORIES(
	${PROJECT_SOURCE_DIR}/sdk/include/tuya_cloud/        
        
	)
		
ADD_LIBRARY(tuya_sys STATIC
	tuya_ipc_motion_detect_demo.c
	adapter_platform.c
	system/tuya_hal_fs.c
	system/tuya_hal_memory.c
	system/tuya_hal_mutex.c
	system/tuya_hal_network.c
	system/tuya_hal_output.c
	system/tuya_hal_p2p_platform_net.c
	system/tuya_hal_semaphore.c
	system/tuya_hal_system.c
	system/tuya_hal_thread.c
	driver/tuya_hal_wifi.c
	driver/tuya_hal_storge.c
	)

