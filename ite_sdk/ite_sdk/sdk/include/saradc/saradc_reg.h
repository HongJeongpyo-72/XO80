#if (CFG_CHIP_FAMILY == 970)
    #include "it970/saradc_reg.h"
#elif (CFG_CHIP_FAMILY == 960)
    #include "it960/saradc_reg.h"
#else
    #error "not defined"
#endif

