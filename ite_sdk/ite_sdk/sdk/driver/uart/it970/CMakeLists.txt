include_directories(
    .
    ..
    ${SMEDIA_INC_DIRS}
    ${PROJECT_SOURCE_DIR}/sdk/include/uart
	${CMAKE_CURRENT_BINARY_DIR}
)

add_library(uart STATIC
    uart.c
)
