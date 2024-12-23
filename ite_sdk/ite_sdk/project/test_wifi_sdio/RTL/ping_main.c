#include "test_config.h"

#if test_ping
void ping_main(void)
{
	printf("====>ping test\n");
	ping_set_target("172.217.160.78"); //midea(101.37.128.72), google(172.217.160.78)
	ping_init();
}
#endif
