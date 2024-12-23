INCLUDE_DIRECTORIES(
    ${SMEDIA_INC_DIRS}
    )

SET(SW_IRRX_SRC
    main.c
    )

ADD_EXECUTABLE(swIRRx.plugin
    ${SW_IRRX_SRC}
)

ADD_CUSTOM_COMMAND(
    TARGET swIRRx.plugin
    POST_BUILD
    COMMAND ${CMAKE_OBJCOPY}
            -O binary -j .risc2_header -j .text -j .data ${CMAKE_CURRENT_BINARY_DIR}/swIRRx.plugin ${LIBRARY_OUTPUT_PATH}/swIRRx.raw
    COMMAND ${CMAKE_OBJDUMP}
            -D ${CMAKE_CURRENT_BINARY_DIR}/swIRRx.plugin > ${LIBRARY_OUTPUT_PATH}/swIRRx.dbg
    COMMAND ${READELF}
            -a ${CMAKE_CURRENT_BINARY_DIR}/swIRRx.plugin > ${CMAKE_CURRENT_BINARY_DIR}/swIRRx.symbol
    COMMAND ${DATACONV} -x ${LIBRARY_OUTPUT_PATH}/swIRRx.raw
    COMMAND mkrom
    ARGS -z -b 512K ${PROJECT_SOURCE_DIR}/sdk/target/ram/${CFG_RAM_INIT_SCRIPT} ${LIBRARY_OUTPUT_PATH}/swIRRx.raw ${LIBRARY_OUTPUT_PATH}/swIRRx.rom
)