if (CFG_CHIP_FAMILY STREQUAL 970 OR CFG_CHIP_FAMILY STREQUAL 960)
    if (DEFINED CFG_M2D_MEMPOOL_ENABLE)
        add_definitions(
            -DCFG_M2D_MEMPOOL_ENABLE
            -DCFG_M2D_MEMPOOL_SIZE=${CFG_M2D_MEMPOOL_SIZE}
        )
    endif()

    include_directories(
    	.
    	..
        ${SMEDIA_INC_DIRS}
        ${PROJECT_SOURCE_DIR}/sdk/include/gfx
        ${PROJECT_SOURCE_DIR}/sdk/share/nedmalloc
    )

    add_library(m2d STATIC
        it970/driver.c
        it970/surface.c
        it970/hw.c
        it970/gfx_mem.c
        it970/gfx_math.c
    )
endif()
