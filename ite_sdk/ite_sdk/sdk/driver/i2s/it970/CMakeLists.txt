ADD_LIBRARY(i2s STATIC
    i2s_switch_GPIO.c
    i2s_reg_970.h
)
if(DEFINED CFG_I2S_CLOCK_FRACTION_MODE)
    ADD_LIBRARY(i2s STATIC
        i2s_970_fraction.c
    )
else()
    ADD_LIBRARY(i2s STATIC
        i2s_970.c
    )
endif()