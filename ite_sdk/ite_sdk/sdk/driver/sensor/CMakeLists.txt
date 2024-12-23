
include_directories(
    .
    ..
    ${SMEDIA_INC_DIRS}
    ${PROJECT_SOURCE_DIR}/sdk/include/sensor
)

add_library(sensor STATIC
    mmp_sensor.c
    adv7180.c
    TVP5150AM1.c
    NT99141.c
	RN6752.c
	RN6854M.c
	GC2145.c
	TP2825.c
    userdefine.c
)

