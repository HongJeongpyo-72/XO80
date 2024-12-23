file(COPY
    ${PROJECT_SOURCE_DIR}/build/$ENV{CFG_BUILDPLATFORM}/risc2/lib/sm32/swIRRx.hex
    DESTINATION ${PROJECT_SOURCE_DIR}/sdk/driver/risc2/swIRRx/.
)

if (DEFINED CFG_RISC2_SW_IRRX AND NOT $ENV{CFG_PLATFORM} STREQUAL win32)
    add_library(swIRRx STATIC
        swIRRx.c
    )
endif()