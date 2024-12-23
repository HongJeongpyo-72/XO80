INCLUDE_DIRECTORIES(
	${PROJECT_SOURCE_DIR}/sdk/include/tuya_cloud/        
        
	)

LINK_LIBRARIES(${PROJECT_SOURCE_DIR}/sdk/share/tuya_cloud/lib_tuya.a)     

		
ADD_LIBRARY(tuya_cloud STATIC
	tuya_ipc_media_demo.c
	tuya_ipc_doorbell_demo.c
	tuya_ipc_system_control_demo.c
	tuya_ipc_sd_demo.c
	#tuya_ipc_wired_demo.c
	tuya_ipc_dp_utils.c
	tuya_ipc_dp_handler.c
	)
		
#target_link_libraries(tuya_cloud ${PROJECT_SOURCE_DIR}/sdk/share/tuya_cloud/lib_tuya.a)
