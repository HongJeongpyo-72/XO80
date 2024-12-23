#include <pthread.h>
#include <errno.h>
#include "lwip/sockets.h"
#include "lwip/ip.h"

#if tcp_server_test
///////////////////////////////////////////
#define PORT "12345"
#define CONNECTIONS 10 //pending connections queue
#define MSG_LEN 256
///////////////////////////////////////////

/* Get sockaddr for IPv4 */
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
        return &(((struct sockaddr_in*)sa)->sin_addr);
}


int network_tcp_server_main(void)
{
    int sockfd, new_fd;
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage sock_addr;
    socklen_t sin_size;
    struct sigaction sa;
    int ec, recv_size, val=1;
    char s[16], msg[MSG_LEN];
    u32_t recv_count = 0;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = PF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags    = AI_PASSIVE; //use host IP

    if ((ec = lwip_getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        printf("getaddrinfo: %s\n", gai_strerror(ec));
        return 1;
    }

    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = lwip_socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
          printf("server: socket create failed\n");
          continue;
        }
        printf("server: socket(%d)\n", sockfd);

        if (lwip_setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(int)) == -1) {
          printf("setsockopt error\n");
            return 1;
        }

        if (lwip_bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
          lwip_close(sockfd);
          printf("server: bind error\n");
          continue;
        } else {
            printf("server: bind(%d)\n", sockfd);
        }

        break;
    }

    if (p == NULL) {
      printf("server: failed to bind\n");
      return 2;
    }

    lwip_freeaddrinfo(servinfo);

    if (lwip_listen(sockfd, CONNECTIONS) == -1) {
      printf("listen failed\n");
        return 3;
    }

    printf("server: waiting for connections...\n");

    /* === Program what you want to do === */
    while(1) {
        /* Accept loop process */
        sin_size = sizeof(sock_addr);
        new_fd = lwip_accept(sockfd, (struct sockaddr *)&sock_addr, &sin_size);
        if (new_fd == -1) {
            printf("accept fd(%d) error\n", sockfd);
            lwip_close(sockfd);
            break;
        } else {
            printf("accept fd(%d) -> new_fd(%d) success\n", sockfd, new_fd);
        }

#if CFG_NET_LWIP_2
        lwip_inet_ntop(sock_addr.ss_family,
                    get_in_addr((struct sockaddr *)&sock_addr), s, sizeof(s));
#else
        if (sock_addr.ss_family == AF_INET)
            ipaddr_ntoa_r(get_in_addr((struct sockaddr *)&sock_addr), s, sizeof(s));
#endif

        printf("server: got connection from %s\n", s);

        if (new_fd > 0) {
            lwip_close(sockfd);

            /* =============== example =============== */
            //Receive a message from client
        	while((recv_size = lwip_recv(new_fd, msg, MSG_LEN, 0)) > 0)
        	{
        	    recv_count++;
        	    printf("server: [%d][%d bytes] msg[%s]\n", recv_count, recv_size, msg);
        		//Send the message back to client
        		msg[recv_size] = '\0';
        		//lwip_write(new_fd, msg, strlen(msg));
        		lwip_send(new_fd, msg, strlen(msg), 0); //lwip_send or lwip_write all both OK
        	}

            if(recv_size == 0)
                printf("Client Disconnected\n");
            else if(recv_size == -1)
                printf("Recv failed\n");

            /* ==================================== */
        }

        lwip_close(new_fd);
    }

    return 0;
}
#endif

#if tcp_client_test
///////////////////////////////////////////
#define SENDER_PORT_NUM 6001
#define SERVER_PORT_NUM 12345
#define SERVER_IP_ADDRESS "192.168.190.102"
///////////////////////////////////////////

int network_tcp_client_main(void)
{
	int socket_fd, connect_fd, data_count=0;
	int recv_timeout;
	int keep_alive_opt = 1, tcp_keepidle = 120, tcp_keepintvl = 10, tcp_keepcnt = 5;
	char * pbuffer = "OK";
	struct sockaddr_in sa, ra;
	struct timeval timeout={5,0}; //5 sec

	int recv_data; char data_buffer[80];
	/* Creates an TCP socket (SOCK_STREAM) with Internet Protocol Family (PF_INET).
	  * Protocol family and Address family related. For example PF_INET Protocol Family and AF_INET family are coupled.
	  */

	socket_fd = lwip_socket(PF_INET, SOCK_STREAM, 0);
	//fcntl(socket_fd, F_SETFL, O_NONBLOCK); //set non-blocking socket

	if ( socket_fd < 0 )
	{

		printf("socket call failed");
		return 0;
	}

	/*memset(&sa, 0, sizeof(struct sockaddr_in));
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = inet_addr(SENDER_IP_ADDR);
	sa.sin_port = htons(SENDER_PORT_NUM);*/


	/* Bind the TCP socket to the port SENDER_PORT_NUM and to the current
	* machines IP address (Its defined by SENDER_IP_ADDR).
	* Once bind is successful for UDP sockets application can operate
	* on the socket descriptor for sending or receiving data.
	*/
	/*if (lwip_bind(socket_fd, (struct sockaddr *)&sa, sizeof(struct sockaddr)) == -1)
	{
		printf("Bind to Port Number %d ,IP address %s failed\n", SENDER_PORT_NUM, SENDER_IP_ADDR);
		lwip_close(socket_fd);
		return 1;
	}*/
	/* Receiver connects to server ip-address. */

	memset(&ra, 0, sizeof(struct sockaddr_in));
	ra.sin_family = AF_INET;
	ra.sin_addr.s_addr = inet_addr(SERVER_IP_ADDRESS);
	ra.sin_port = htons(SERVER_PORT_NUM);

	sleep(5);

	connect_fd = lwip_connect(socket_fd, (struct sockaddr*)&ra, sizeof(ra));

	if(connect_fd < 0)
	{
		printf("connect failed: %d \n", connect_fd);
		lwip_close(socket_fd);
		return 0;
	}

	//recv_timeout = lwip_setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
	lwip_setsockopt(socket_fd, SOL_SOCKET, SO_KEEPALIVE, (void*)&keep_alive_opt, sizeof(keep_alive_opt));
	lwip_setsockopt(socket_fd, IPPROTO_TCP, TCP_KEEPIDLE, (void*)&tcp_keepidle, sizeof(tcp_keepidle));
	lwip_setsockopt(socket_fd, IPPROTO_TCP, TCP_KEEPINTVL, (void*)&tcp_keepintvl, sizeof(tcp_keepintvl));
	lwip_setsockopt(socket_fd, IPPROTO_TCP, TCP_KEEPCNT, (void*)&tcp_keepcnt, sizeof(tcp_keepcnt));

	while (1){
        recv_data = lwip_recv(socket_fd, data_buffer, sizeof(data_buffer), 0);
		++data_count;

		if(recv_data == -1 && errno == EAGAIN)
		{
		    printf("timeout\n");
		}

	    if(recv_data > 0)
        {
            data_buffer[recv_data] = '\0';
            printf("received data[%d bytes][%d]: %s \n", recv_data, data_count, data_buffer);

			lwip_send(socket_fd, pbuffer, strlen(pbuffer), 0);
		}else{
		    printf("recv failed \n");
		    lwip_close(socket_fd);
		    return 0;
	    }
	}
}
#endif

