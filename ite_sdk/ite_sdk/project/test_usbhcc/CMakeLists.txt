add_executable(${CMAKE_PROJECT_NAME}
    test_$ENV{CFG_PLATFORM}.c
)

if (DEFINED CFG_TEST_MST)
    add_executable(${CMAKE_PROJECT_NAME}
        test_usb_mst.c
    )
elseif (DEFINED CFG_TEST_SD)
    add_executable(${CMAKE_PROJECT_NAME}
        test_card.c
    )
elseif (DEFINED CFG_TEST_MASS_STORAGE)
    add_executable(${CMAKE_PROJECT_NAME}
        test_usbd_fsg.c
    )
elseif (DEFINED CFG_USBH_CD_CDCACM)
    ITE_ADD_DEFINITIONS_IF_DEFINED(CFG_TEST_USBH_CDCACM_SEND_FILE)
    ITE_ADD_DEFINITIONS_IF_DEFINED_STRING_VALUE(CFG_USBH_ACM_FILENAME)
    
    add_executable(${CMAKE_PROJECT_NAME}
        test_usbh_acm.c
    )
elseif (DEFINED CFG_USBD_CD_CDCACM)
    ITE_ADD_DEFINITIONS_IF_DEFINED(CFG_TEST_ACM_LOOPBACK)
    ITE_ADD_DEFINITIONS_IF_DEFINED(CFG_TEST_ACM_SEND_FILE)

    add_executable(${CMAKE_PROJECT_NAME}
        test_usbd_acm.c
    )
elseif (DEFINED CFG_USBD_CD_MST)
    add_executable(${CMAKE_PROJECT_NAME}
        test_usbd_mst.c
    )
elseif (DEFINED CFG_TEST_HID_HOST)
    add_executable(${CMAKE_PROJECT_NAME}
        test_usb_hid_host.c
    )
elseif (DEFINED CFG_TEST_HID_DEVICE)
    add_executable(${CMAKE_PROJECT_NAME}
        test_usb_hid_device.c
    )
endif()

# build
include(${PROJECT_SOURCE_DIR}/sdk/build.cmake)
