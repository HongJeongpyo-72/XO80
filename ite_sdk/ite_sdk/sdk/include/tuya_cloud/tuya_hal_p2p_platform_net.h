/**
 * @file tuya_p2p_net.h
 * @author chentong@tuya.com
 * @brief 网络接口封装文件
 * @version 0.1
 * @date 2019-10-26
 * 
 * @copyright Copyright (c) tuya.inc 2019
 * 
 */


#ifndef __TUYA_HAL_P2P_PLATFORM_NET_H__
#define __TUYA_HAL_P2P_PLATFORM_NET_H__

#include <stdint.h>
#include <stdbool.h>

#define TUYA_P2P_RTOS_SOCKET_H    1

#define TUYA_RTOS_3905_PLATFORM   1

#ifdef TUYA_P2P_RTOS_SOCKET_H

#ifdef TUYA_RTOS_3905_PLATFORM
#include "sys/socket.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "lwip/inet.h"
#include <signal.h>
#endif

//#include "tuya_hal_system.h"
//#include "tuya_hal_mutex.h"

#ifdef __cplusplus
    extern "C" {
#endif

///////////////////////////////////////////////////////////////////////
#ifndef TUYA_RTOS_IPV6
#define INET6_ADDRSTRLEN	46
#endif

//////////////////////////////////////////////////////////////////////
struct STUYA_interface_address_s {
  char* name;
  char phys_addr[6];
  int is_internal;
  union {
    struct sockaddr_in address4;
	#ifdef TUYA_RTOS_IPV6
    struct sockaddr_in6 address6;
	#endif
  } address;
  union {
    struct sockaddr_in netmask4;
	#ifdef TUYA_RTOS_IPV6
    struct sockaddr_in6 netmask6;
	#endif
  } netmask;
};

////////////////////////////////////////////////////////////////////////////
/***********************************************************
*  Function: tuya_p2p_getlocal_NatIp
*  Desc:     获取本地的IP地址.
*  Output:   p_out_ip,输出参数，返回当前本地的IP地址.
*  Return:   1:表示成功.	0:表示失败.
***********************************************************/
int tuya_p2p_getlocal_NatIp(char *p_out_ip);

/***********************************************************
*  Function: tuya_p2p_lib_getsocketpeer
*  Desc:     从sock_fd套接字获取对端地址信息.
*  Return:   0:Successful;  <0:Failed.
***********************************************************/
int tuya_p2p_lib_getsocketpeer(int sock_fd);

/***********************************************************
*  Function: uv_inet_pton
*  Desc:     网络标准转换接口,pton 地址转换函数
*  Input:    af,输入参数，IPV6或IPV4协议族.
*  Input:    src,输入参数，IP地址信息(127.0.0.1)
*  Output:   dst,输出参数，网络序列化的二进制的IP地址信息.
*  Return:   <0:表示成功.	0:表示指定的协议族不正确. >0:表示失败.
***********************************************************/
int tuya_p2p_lib_inet_pton(int af, const char *src, struct sockaddr_storage *dst);

/***********************************************************
*  Function: uv_inet_ntop
*  Desc:     网络标准转换接口,ntop 地址转换函数
*  Input:    af,输入参数，IPV6或IPV4协议族.
*  Input:    src,输入参数，网络序列化的二进制的IP地址信息
*  Output:   dst,输出参数，IP地址信息(127.0.0.1).
*  Output:   szlen,输出参数,返回ipv6地址信息的长度值.
*  Return:   <0:表示成功.	0:表示指定的协议族不正确. >0:表示失败.
***********************************************************/
char* tuya_p2p_lib_inet_ntop(int af, struct sockaddr_storage *src, char *dst, socklen_t szlen);

/***********************************************************
*  Function: uv_ip4_addr
*  Desc:     将输入的ip地址及端口转成IPV4的网络地址结构.
*  Input:    p_in_ipv4,输入参数，IPV4的地址信息(127.0.0.1)
*  Input:    m_in_port,输入参数，IPV4对应的端口号
*  Output:   p_out_addr,输出参数，网络IPV4的地址结构体.
*  Return:   void
***********************************************************/
void tuya_p2p_lib_ip4_addr(const char *p_in_ipv4, int m_in_port, struct sockaddr_in *p_out_addr);

/***********************************************************
*  Function: uv_ip4_name
*  Desc:     根据IPV4的网络地址结构返回对应的IPV4地址字符串.
*  Input:    p_in_addr,输入参数，网络序列化的二进制的IP地址信息
*  Output:   p_out_ipv4,输出参数，IPV4的地址信息(127.0.0.1)
*  Input:    m_in_len,输入参数，需要输出IPV4地址的长度值.
*  Return:   void
***********************************************************/
void tuya_p2p_lib_ip4_name(const struct sockaddr_in *p_in_addr, char *p_out_ipv4, int m_in_len);

#ifdef TUYA_RTOS_IPV6
void tuya_p2p_lib_ip6_addr(const char *p_in_ipv6, int m_in_port, struct sockaddr_in6 *p_out_addr);
void tuya_p2p_lib_ip6_name(const struct sockaddr_in6 *p_in_addr, char *p_out_ipv6, int m_in_len);
#endif

#endif	//USE TUYA_P2P_RTOS_SOCKET_H

#ifdef __cplusplus
}
#endif

#endif // __TUYA_HAL_P2P_NET_PART_H__