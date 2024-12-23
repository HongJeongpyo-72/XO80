include_directories(
    ${PROJECT_SOURCE_DIR}/sdk/driver/usb/inc/
    ${PROJECT_SOURCE_DIR}/sdk/driver/
)

if (DEFINED CFG_HAVE_USBLP)
    add_library(usblp STATIC
      usblp.c
    )
endif()
