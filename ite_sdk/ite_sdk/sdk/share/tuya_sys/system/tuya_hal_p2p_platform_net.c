#include <assert.h>

#include "tuya_hal_p2p_platform_net.h"

#ifdef TUYA_P2P_RTOS_SOCKET_H

#include "tuya_hal_system.h"
#include "tuya_hal_mutex.h"
#include <netdb.h>

#ifdef _LINUX_
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#endif

//
//global variable define
//

#ifdef TUYA_RTOS_3905_PLATFORM
extern struct netif *netif_list;
#endif

//
//function define
//

/***********************************************************
*  Function: tuya_p2p_getlocal_NatIp
*  Desc:     获取本地的IP地址.
*  Output:   p_out_ip,输出参数，返回当前本地的IP地址.
*  Return:   1:表示成功.	0:表示失败.
***********************************************************/
int tuya_p2p_getlocal_NatIp(char *p_out_ip)
{	
	return fill_my_ip(p_out_ip);

#if 0 //#ifdef TUYA_RTOS_3905_PLATFORM
	struct netif *pnetif = netif_list;

	if (pnetif == NULL)
			return 0;
	
	while (1) 
	{
		if (pnetif == NULL)
			break;
	
		strcpy(m_ipv4_str,ipaddr_ntoa(&pnetif->ip_addr));
		if (strcmp(m_ipv4_str,"0.0.0.0") != 0)
		{
			strcpy(p_out_ip,m_ipv4_str);
			return 1;
		}
		
		pnetif = pnetif->next; 
	}
#endif

#ifdef _LINUX_

#define NI_MAXHOST 1025

	struct ifaddrs *ifaddr, *ifa;
    int family, s;
    char host[NI_MAXHOST] = {0};
 
    if (getifaddrs(&ifaddr) == -1) {    //通过getifaddrs函数得到所有网卡信息
        perror("getifaddrs");            //通过struct ifaddr结构体由指针，将所有信息通过链表连接
        return 0;
    }
 
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {    //做链表做循环
        if (ifa->ifa_addr == NULL)                            //判断地址是否为空
            continue;
 
        family = ifa->ifa_addr->sa_family;                    //得到IP地址的协议族 
        if (family == AF_INET || family == AF_INET6) 
		{
			//判断协议族是AF_INET还是AF_INET6
        	//通过getnameinfo函数得到对应的IP地址。NI_MAXHOST为宏定义，值为1025. NI_NUMERICHOST宏定义，和NI_NUMERICSERV对应，都试一下就知道了。
            s = getnameinfo(ifa->ifa_addr,
                    (family == AF_INET) ? sizeof(struct sockaddr_in) :
                    sizeof(struct sockaddr_in6),
                    host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
            if (s != 0) 
			{
                printf("getnameinfo() failed: %s\n", gai_strerror(s));
                return 0;
            }

			//类似获取第一个不为全0的IP地址.
			if (strcmp(host,"0.0.0.0") != 0)
			{
				strcpy(p_out_ip,host);
				freeifaddrs(ifaddr);
				return 1;
			}            
        }
    }
 
    freeifaddrs(ifaddr);
#endif

	return 0;
}

/***********************************************************
*  Function: tuya_p2p_lib_getHostByName
*  Desc:     将ip地址或域名信息转换成UINT32的IP地址信息.
*  Input:    p_in_dns_name,输入参数，IPV4协议族的IP地址或者域名信息.
*  Output:   NULL
*  Return:   0:表示失败，转换成IP地址失败.	>0:表示获取IP地址信息成功.
***********************************************************/
uint32_t tuya_p2p_lib_getHostByName(char *p_in_dns_name)
{
	uint32_t dns = 0;
	struct hostent *hp = NULL;
	
	dns = inet_addr(p_in_dns_name);
	
	if (dns == (uint32_t) -1) 
	{
		if ((hp = gethostbyname(p_in_dns_name)) == NULL)
		{
			printf("[libuv] getHostByName: %s Error!\n",p_in_dns_name);
			return 0;
		}
	
		dns = *(uint32_t *)hp->h_addr;
	}

	return dns;
}

/***********************************************************
*  Function: tuya_p2p_lib_getsocketpeer
*  Desc:     从sock_fd套接字获取对端地址信息.
*  Return:   0:Successful;  <0:Failed.
***********************************************************/
int tuya_p2p_lib_getsocketpeer(int sock_fd)
{
	struct sockaddr remotPeerAdress;
    int remotPeerAdressLen = sizeof(remotPeerAdress);
    if(getpeername(sock_fd, (struct sockaddr *)&remotPeerAdress, &remotPeerAdressLen) != -1)
    {
        return 0;
    }

	return -1;
}

/***********************************************************
*  Function: tuya_p2p_lib_inet_pton
*  Desc:     网络标准转换接口,pton 地址转换函数
*  Input:    af,输入参数，IPV6或IPV4协议族.
*  Input:    src,输入参数，IP地址信息(127.0.0.1)
*  Output:   dst,输出参数，网络序列化的二进制的IP地址信息.
*  Return:   <0:表示成功.	0:表示指定的协议族不正确. >0:表示失败.
***********************************************************/
int tuya_p2p_lib_inet_pton(int af, const char *src, struct sockaddr_storage *dst)
{
	int n_ret = inet_pton(af,src,(void *)dst);
	return n_ret;
}

/***********************************************************
*  Function: tuya_p2p_lib_inet_ntop
*  Desc:     网络标准转换接口,ntop 地址转换函数
*  Input:    af,输入参数，IPV6或IPV4协议族.
*  Input:    src,输入参数，网络序列化的二进制的IP地址信息
*  Output:   dst,输出参数，IP地址信息(127.0.0.1).
*  Output:   szlen,输出参数,返回ipv6地址信息的长度值.
*  Return:   <0:表示成功.	0:表示指定的协议族不正确. >0:表示失败.
***********************************************************/
char* tuya_p2p_lib_inet_ntop(int af, struct sockaddr_storage *src, char *dst, socklen_t szlen)
{
	return inet_ntop(af,(void *)dst,src,szlen);
}

/***********************************************************
*  Function: tuya_p2p_lib_ip4_addr
*  Desc:     将输入的ip地址及端口转成IPV4的网络地址结构.
*  Input:    p_in_ipv4,输入参数，IPV4的地址信息(127.0.0.1)
*  Input:    m_in_port,输入参数，IPV4对应的端口号
*  Output:   p_out_addr,输出参数，网络IPV4的地址结构体.
*  Return:   void
***********************************************************/
void tuya_p2p_lib_ip4_addr(const char *p_in_ipv4,int m_in_port,struct sockaddr_in *p_out_addr)
{
	p_out_addr->sin_family = AF_INET;
	p_out_addr->sin_port = htons(m_in_port);

	//change domainame to ip-addr.
	uint32_t m_out_ip = tuya_p2p_lib_getHostByName(p_in_ipv4);
	p_out_addr->sin_addr.s_addr = m_out_ip;
}

/***********************************************************
*  Function: tuya_p2p_lib_ip4_name
*  Desc:     根据IPV4的网络地址结构返回对应的IPV4地址字符串.
*  Input:    p_in_addr,输入参数，网络序列化的二进制的IP地址信息
*  Output:   p_out_ipv4,输出参数，IPV4的地址信息(127.0.0.1)
*  Input:    m_in_len,输入参数，需要输出IPV4地址的长度值.
*  Return:   void
***********************************************************/
void tuya_p2p_lib_ip4_name(const struct sockaddr_in *p_in_addr, char *p_out_ipv4, int m_in_len)
{
	inet_ntop(AF_INET, &p_in_addr->sin_addr, p_out_ipv4, m_in_len);
}

#ifdef TUYA_RTOS_IPV6
void tuya_p2p_lib_ip6_addr(char *p_in_ipv6, int m_in_port, struct sockaddr_in6 *p_out_addr)
{
	p_out_addr->sin6_family = AF_INET6;
	p_out_addr->sin6_port = htons(m_in_port);
	//need to change domainame to ip
	inet_pton(AF_INET6, p_in_ipv6, (void *)&p_out_addr->sin6_addr);  // IPV6
}

void tuya_p2p_lib_ip6_name(const struct sockaddr_in6 *p_in_addr, char *p_out_ipv6, int m_in_len)
{
	inet_ntop(AF_INET6, &p_in_addr->sin_addr, p_out_ipv6, m_in_len);
}

#endif 

#endif// USE TUYA_P2P_RTOS_SOCKET_H
