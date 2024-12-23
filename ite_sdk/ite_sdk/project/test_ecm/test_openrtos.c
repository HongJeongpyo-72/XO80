#include <pthread.h>
#include "openrtos/FreeRTOS.h"
#include "openrtos/task.h"
#include "ite/itp.h"

extern void* Test4G(void* arg);
extern void* TestEthernet(void* arg);

void* TestFunc(void* arg)
{
    pthread_t task;
    pthread_attr_t attr;

    itpInit();

    pthread_attr_init(&attr);
    pthread_create(&task, &attr, Test4G, NULL);

#if defined(CFG_TEST_ETHERNET)
    pthread_create(&task, &attr, TestEthernet, NULL);
#endif

    while (1) 
        sleep(1);

}

int main(void)
{
    pthread_t task;
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_create(&task, &attr, TestFunc, NULL);

    /* Now all the tasks have been started - start the scheduler. */
    vTaskStartScheduler();

    /* Should never reach here! */
    return 0;
}
