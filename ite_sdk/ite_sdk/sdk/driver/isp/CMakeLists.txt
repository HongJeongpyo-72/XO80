﻿if (CMAKE_BUILD_TYPE STREQUAL Debug)
    ADD_DEFINITIONS(
        -DDEBUG
    )
endif()

if (CFG_CHIP_FAMILY STREQUAL 970 OR CFG_CHIP_FAMILY STREQUAL 960)
    add_subdirectory(it970)
endif()
