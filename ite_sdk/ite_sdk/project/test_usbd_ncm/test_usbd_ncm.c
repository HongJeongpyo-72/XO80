#include <sys/ioctl.h>
#include <assert.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "lwip/ip.h"
#include "ping.h"
#include "curl/curl.h"
#include "ite/itp.h"


static int NetworkInit(void)
{
    ITPEthernetSetting setting = { 0 };
    ITPEthernetInfo info = { 0 };
    char ip[16] = { 0 };

    /* wait link up */
    while (!ioctl(ITP_DEVICE_ETHERNET, ITP_IOCTL_IS_CONNECTED, NULL)) {
        sleep(1);
        putchar('.');
        fflush(stdout);
    }
    printf("Network connected.. \n");

    /* static IP */
    setting.dhcp = 0;
    // ipaddr
    setting.ipaddr[0] = 192;
    setting.ipaddr[1] = 168;
    setting.ipaddr[2] = 2;
    setting.ipaddr[3] = 100;
    // netmask
    setting.netmask[0] = 255;
    setting.netmask[1] = 255;
    setting.netmask[2] = 255;
    setting.netmask[3] = 0;
    // gateway
    setting.gw[0] = 192;
    setting.gw[1] = 168;;
    setting.gw[2] = 2;
    setting.gw[3] = 254;
    ioctl(ITP_DEVICE_ETHERNET, ITP_IOCTL_RESET, &setting);

    /* wait for get ip ready */
    while (!ioctl(ITP_DEVICE_ETHERNET, ITP_IOCTL_IS_AVAIL, NULL))
        usleep(1000);

    /* get IP address */
    info.index = 0;
    ioctl(ITP_DEVICE_ETHERNET, ITP_IOCTL_GET_INFO, &info);
    ipaddr_ntoa_r((const ip_addr_t*)&info.address, ip, sizeof(ip));
    printf("\n\nGet IP: %s \n\n", ip);

    return 0;
}

#if defined(CFG_TEST_PING)

static void DoPing(void)
{
    //printf("\n\nping: 172.217.160,78 \n\n");
    //ping_set_target("172.217.160.78"); // google(172.217.160.78)
    printf("\n\nping: %s \n\n", CFG_PING_IP);
    ping_set_target(CFG_PING_IP);
    ping_init();
}

#endif // #if defined(CFG_TEST_PING)


//====================================
#if defined(CFG_TEST_SOCKET)

#define NCM_BUF_SIZE            (64*1024)
#define NCM_BUF_NUM                     100

#define SOCKET_INIT               0
#define SOCKET_CONNECT            1
#define SOCKET_CONNECT_SUCCESS    2
#define SOCKET_DISCONNECT         3

#define  HU_IP_PORT              10001
#define  HU_IP_ARRD              "192.168.2.99"


typedef struct _NCM_config_ {
    struct sockaddr_in socket_addr;
    int socket_fd;
    int socket_status;
} NCM_config_t;

static NCM_config_t ncm_cfg = { 0 };
static char tx_buffer[1500];
static char rx_buffer[4096];

void* rx_task(void* arg)
{
    NCM_config_t *config = (NCM_config_t*)arg;
    int rx_len;

    while (config->socket_status != SOCKET_CONNECT_SUCCESS)
        usleep(1000);

    while (1) {
        rx_len = recv(config->socket_fd, rx_buffer, 4096, 0);
        printf("[NCM] Rx len: %d \n", rx_len);
    }
}

int Keep_Socket_Connect_Status(NCM_config_t *config)
{
    int ret;
    struct timeval recv_timeout;

    recv_timeout.tv_sec = 1000;
    recv_timeout.tv_usec = 0;

    while (1)
    {
        if (config->socket_status != SOCKET_CONNECT_SUCCESS)
        {
            if (config->socket_status != SOCKET_INIT)
            {
                config->socket_status = SOCKET_INIT;
                close(config->socket_fd);
            }
            bzero(&config->socket_addr, sizeof(config->socket_addr));
            config->socket_addr.sin_family = AF_INET;
            config->socket_addr.sin_port = htons(HU_IP_PORT);
            ret = inet_pton(AF_INET, HU_IP_ARRD, &config->socket_addr.sin_addr);
            if (ret < 0)
            {
                printf("error ip address %s!\n", HU_IP_ARRD);
            }

            config->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
            if (config->socket_fd != -1)
            {
                config->socket_status = SOCKET_CONNECT;
            }
            else
            {
                printf("Creating socket failed.\n");
                usleep(100000);
            }
            setsockopt(config->socket_fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&recv_timeout, sizeof(struct timeval));
        }

        if (config->socket_status == SOCKET_CONNECT)
        {
            ret = connect(config->socket_fd, (struct sockaddr *)&config->socket_addr, sizeof(struct sockaddr));
            if (ret != -1)
            {
                printf("connect server\n");
                config->socket_status = SOCKET_CONNECT_SUCCESS;
                break;
            }
            else
            {
                usleep(500000);
                //                              printf("----disconnect server\n");
            }
        }
    }

    if (1) // Tx: send data
    {
        uint32_t i, tx_cnt = 0;
        tx_buffer[0] = 0x11;
        tx_buffer[1] = 0x22;
        tx_buffer[2] = 0x33;
        tx_buffer[3] = 0x44;
        tx_buffer[1496] = 0x55;
        tx_buffer[1497] = 0x66;
        tx_buffer[1498] = 0x77;
        tx_buffer[1499] = 0x88;
        printf("send data: \n");
        while (1) {
            usleep(20*1000);
            send(config->socket_fd, tx_buffer, 1500, 0);
            if (!(++tx_cnt % 5))
                printf("\nSend Tx len: %d \n\n", (1500 * 5));
        }
    }

    return 0;
}

#endif
//====================================


void* TestFunc(void* arg)
{
    int rc;

    rc = NetworkInit();
    if (rc) {
        printf("NetworkInit() fail! \n");
        while (1);
    }
#if defined(CFG_TEST_NONE)

#elif defined(CFG_TEST_PING)
    DoPing();
#elif defined(CFG_TEST_SOCKET)
    {
        pthread_t task;
        pthread_attr_t attr;

        pthread_attr_init(&attr);
        pthread_create(&task, &attr, rx_task, &ncm_cfg);

        Keep_Socket_Connect_Status(&ncm_cfg);
    }
#endif

    while (1)
        sleep(1);
}


