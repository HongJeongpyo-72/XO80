add_executable(${CMAKE_PROJECT_NAME}
    test_$ENV{CFG_PLATFORM}.c
    )

include_directories(
    ${PROJECT_SOURCE_DIR}/sdk/share/ping/
    )
 
if (DEFINED CFG_TEST_PING)
    add_definitions(
        -DCFG_TEST_PING
        -DCFG_PING_IP="${CFG_PING_IP}"
    )
elseif (DEFINED CFG_TEST_HTTP)
    add_definitions(
        -DCFG_TEST_HTTP
        -DCFG_HTTP_URL="${CFG_HTTP_URL}"
    )
elseif (DEFINED CFG_TEST_FTP_UPLOAD)
    add_definitions(
        -DCFG_TEST_FTP_UPLOAD
        -DCFG_FTP_URL="${CFG_FTP_URL}"
    )
endif()    

if (DEFINED CFG_TEST_ETHERNET)
  add_definitions(
        -DCFG_TEST_ETHERNET
    )
endif()    


if (DEFINED CFG_NET_N720_4G)
	add_executable(${CMAKE_PROJECT_NAME}
	    test_dial_N720.c
	    )
elseif (DEFINED CFG_NET_L718_4G)	    
	add_executable(${CMAKE_PROJECT_NAME}
	    test_dial_L718.c
	    )
elseif (DEFINED CFG_NET_L610_4G)	    
	add_executable(${CMAKE_PROJECT_NAME}
	    test_dial_L610.c
	    )
elseif (DEFINED CFG_NET_AIR720SL_4G)	    
	add_executable(${CMAKE_PROJECT_NAME}
	    test_dial_Air720SL.c
	    )
elseif (DEFINED CFG_NET_M8910DL_4G)	    
	add_executable(${CMAKE_PROJECT_NAME}
	    test_dial_M8910DL.c
	    )
endif()

add_executable(${CMAKE_PROJECT_NAME}
    test_ecm.c
    )
    
if (DEFINED CFG_TEST_FTP_UPLOAD)
	add_executable(${CMAKE_PROJECT_NAME}
	    ftpupload.c
	    )
endif()

if (DEFINED CFG_TEST_ETHERNET)
	add_executable(${CMAKE_PROJECT_NAME}
	    test_ethernet.c
	    )
endif()    


# build
include(${PROJECT_SOURCE_DIR}/sdk/build.cmake)
