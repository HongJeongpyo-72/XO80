#if (CFG_CHIP_FAMILY == 970)
    #include "it970/saradc.h"
#elif (CFG_CHIP_FAMILY == 960)
    #include "it960/saradc.h"
#else
    #error "not defined"
#endif

