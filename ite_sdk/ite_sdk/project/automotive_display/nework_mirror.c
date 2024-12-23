#include <sys/ioctl.h>
#include <sys/socket.h>
#include <pthread.h>
#include <time.h>
#include <assert.h>
#include "lwip/ip.h"
#include "lwip/dns.h"
#include "ite/itp.h"
#include "ite/itv.h"
#include "ith/ith_video.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"

#define RECV_BUF_LEN    2*1024
#define NAL_BUF_LEN    512*1024

#define MACRO_ERTN         { \
        printf("%f:%d, error returned!\n", __FILE__, __LINE__); \
        return (void *)-1; \
}

#define MACRO_NRTN         { \
        printf("%f:%d, error returned!\n", __FILE__, __LINE__); \
        return; \
}

static int  sockfd = 0, forClientSockfd = 0;
static bool mirror_is_init = false, mirror_from_H = false, network_mirror_start = false, backup_is_init = false, backup_recving = false;
static pthread_t recv_thread, backup_recv_thread;
static uint32_t  lastTick = 0;
static uint8_t   window_state = 0;

static void display_on_LCD(AVFrame *picture)
{
    ITV_DBUF_PROPERTY prop = {0};

    //while (1)
    {
        uint8_t *dbuf = itv_get_dbuf_anchor();
        if (dbuf != NULL)
        {
            uint32_t rdIndex = picture->opaque ? *(uint32_t *)picture->opaque : 0, check_cnt = 0;
            //uint8_t  *ya = picture->data[0];

            //printf("+++++++++++++++(%d, %d, %d, %d)\n", picture->width, picture->height, picture->linesize[0], picture->linesize[1]);
            //printf("(0x%x, 0x%x, 0x%x)\n", ya[0], ya[picture->width/2], ya[picture->width]);
            //for(int i = 0; i < picture->width/2; i++)
            //{
                //printf("(0x%x)\n", ya[picture->width*picture->height/2+i]);
                //if(ya[picture->width*picture->height/2+i] == 0x10)
                //    check_cnt++;
            //}
            //printf("\n\n");
            //if(check_cnt < picture->width/4)
            if(mirror_from_H)
            {
                if(window_state != 1)
                {
                    printf("show_mirror_window_W!!!!!!!!!\n");
                    show_mirror_window(1280, 480);
                }
                window_state = 1;
                prop.src_w    = picture->width;
                prop.src_h    = picture->height;
                prop.ya       = picture->data[0];
                prop.ua       = picture->data[1];
                prop.va       = picture->data[2];
                prop.pitch_y  = picture->linesize[0];
                prop.pitch_uv = picture->linesize[1];
                prop.bidx     = rdIndex;
                prop.format   = MMP_ISP_IN_NV12;
            }else
            {
                if(window_state != 2)
                {
                    printf("show_mirror_window_H!!!!!!!!!\n");
                    show_mirror_window(480, 1280);
                }
                window_state = 2;
                prop.src_w    = picture->width;
                prop.src_h    = picture->height;
                prop.ya       = picture->data[0];
                prop.ua       = picture->data[1];
                prop.va       = picture->data[2];
                prop.pitch_y  = picture->linesize[0];
                prop.pitch_uv = picture->linesize[1];
                prop.bidx     = rdIndex;
                prop.format   = MMP_ISP_IN_NV12;
            }

            itv_update_dbuf_anchor(&prop); // printf("display mark non-use %d\n", rdIndex);
            //break;
        }
        else
        {
            ithDelay(1000);
        }
    }
}

#if 0
static void* tcp_mirror_task(void* arg)
{
    uint8_t* recvBuf = (uint8_t*) malloc(RECV_BUF_LEN);
    uint32_t recv_size = 0;
    
    // ffmpeg
    AVCodec        *codec;
    AVCodecContext *c       = NULL;
    AVFrame        *picture = NULL;
    AVDictionary   *dict    = NULL;
    // ffmpeg
    AVPacket       avpkt;
    int            got_picture = 0, len = 0;

#ifdef CFG_VIDEO_ENABLE
	ituFrameFuncInit();
#endif // CFG_VIDEO_ENABLE
    itv_set_video_window(0, 0, ithLcdGetWidth(), ithLcdGetHeight());
    itv_set_pb_mode(1);

    avcodec_register_all(); // avcodec_init();
    codec = avcodec_find_decoder(CODEC_ID_H264);
    if (!codec)
    {
        MACRO_ERTN;
    }
    c               = avcodec_alloc_context3(codec); // avcodec_alloc_context();

#ifdef _DEBUG
    c->debug        = FF_DEBUG_PICT_INFO | FF_DEBUG_SKIP | FF_DEBUG_STARTCODE | FF_DEBUG_PTS | FF_DEBUG_MMCO | FF_DEBUG_BUGS | FF_DEBUG_BUFFERS | FF_DEBUG_THREADS;
#endif

    picture         = avcodec_alloc_frame();
    assert(picture);
    avcodec_get_frame_defaults(picture);
    picture->opaque = malloc(sizeof(uint32_t));
    if (codec->capabilities & CODEC_CAP_TRUNCATED)
        c->flags |= CODEC_FLAG_TRUNCATED;

    if (avcodec_open2(c, codec, &dict) /*avcodec_open(c, codec)*/ < 0)
    {
        MACRO_ERTN;
    }
#ifdef _DEBUG
    av_log_set_level(AV_LOG_DEBUG);
#endif

    while(1)
    {
        recv_size = recv(forClientSockfd,recvBuf,sizeof(recvBuf),0);
        printf("====>network_mirror size=%d, data<=========\n", recv_size);
        for(int i = 0; i<recv_size; i++)
        {
            printf("%x,",recvBuf[i]);
        }
        printf("\n");
        //if(recv_size > 0)
        //{
        //    avpkt.data = NULL;
        //    avpkt.size = 0;

        //    len = avcodec_decode_video2(c, picture, &got_picture, &avpkt);
        //    if (got_picture)
        //    {
        //        display_on_LCD(picture);
        //    }
        //}
        usleep(1000);
    }

    if(recvBuf)
        free(recvBuf);
    if(forClientSockfd)
        close(forClientSockfd);
    // ffmpeg de-initial
    avcodec_close(c);
    av_free(c);
    if (picture->opaque)
        free(picture->opaque);
    av_free(picture);
    itv_set_pb_mode(0);
    
    pthread_exit(NULL);
    return NULL;
}

void network_mirror_start(void)
{
    uint8_t* recvBuf = (uint8_t*) malloc(RECV_BUF_LEN);
    uint32_t recv_size = recv(forClientSockfd,recvBuf,sizeof(recvBuf),0);
    //printf("====>network_mirror_start size=%d, data<=========\n", recv_size);
    for(int i = 0; i<recv_size; i++)
    {
        printf("%x,",recvBuf[i]);
    }
    return 0;
}

void network_mirror_init(void)
{
    if(mirror_is_init)
    {
        //network_mirror_start();
        return 0;
    }
    
    char message[] = {"OK"};
    struct sockaddr_in serverInfo,clientInfo;
    int addrlen = sizeof(clientInfo);
    struct timeval timeout = {0};
    fd_set readfds;
    
    sockfd = socket(AF_INET , SOCK_STREAM , 0);

    if (sockfd == -1){
        printf("Fail to create a socket.\n");
        return 0;
    }
    //socket link
    memset(&serverInfo,'\0',sizeof(struct sockaddr_in));
    //memset(&clientInfo,'\0',sizeof(struct sockaddr_in));
    
    serverInfo.sin_family = PF_INET;
    serverInfo.sin_addr.s_addr = INADDR_ANY;
    serverInfo.sin_port = htons(11111);
    if(bind(sockfd,(struct sockaddr *)&serverInfo,sizeof(serverInfo)) < 0)
    {
        printf("socket bind failed\n");
        return 0;
    }
    // listen for incoming connections (TCP listen backlog = 5)
    listen(sockfd,5);
    printf("====>network_mirror_checking\n");
    //int err = connect(sockfd,(struct sockaddr *)&clientInfo,sizeof(serverInfo));
    //if(err==-1){
    //    printf("Connection error\n");
    //}
    
    /*set receive form return side with timeout*/
    FD_ZERO(&readfds);
    FD_SET(sockfd,&readfds);
    timeout.tv_sec = 0;
    timeout.tv_usec = 100000;
    select(sockfd+1,&readfds,NULL,NULL,&timeout);
    if(FD_ISSET(sockfd,&readfds))
    {
        forClientSockfd = accept(sockfd,(struct sockaddr*) &clientInfo, &addrlen);
        send(forClientSockfd,message,sizeof(message),0);
    
        mirror_is_init = true;
        pthread_create(&recv_thread, NULL, tcp_mirror_task, NULL);
        printf("====>network_mirror_start\n");
    }
    close(sockfd);
    sockfd = 0;
    return 0;
}
#endif

static void init_codec(void)
{
}

static void* udp_backup_task(void* arg)
{
    fd_set readfds;
    struct timeval timeout = {0};
    uint32_t recv_size = 0;
    uint8_t* recvBuf = (uint8_t*) malloc(RECV_BUF_LEN);

    backup_is_init = true;
    
    while(1)
    {
        //printf("++++++++++\n");
        if(!network_mirror_start)
        {
            backup_recving = true;
            if(mirror_is_init)
            {    
                if(sockfd)
                    close(sockfd);
                backup_recving = false;
            }
            else
            {
            //printf("+++++++++++111111\n");
            FD_ZERO(&readfds);
		    FD_SET(sockfd,&readfds);
		    timeout.tv_sec = 0;
		    timeout.tv_usec = 5000;
		    select(sockfd+1,&readfds,NULL,NULL,&timeout);
            if(FD_ISSET(sockfd,&readfds))
            {
                recv_size = recv(sockfd, recvBuf, RECV_BUF_LEN, 0);
                printf("UDP_BACK recv = %d\n", recv_size);
            }
        }
        }
        usleep(1000);
    }
    if(recvBuf)
        free(recvBuf);
    
    //pthread_exit(NULL);
   
    return NULL;
}

static void* udp_mirror_task(void* arg)
{
    FILE* f = NULL;
    int k = 0;
    uint8_t  message[13] = {0x4F, 0x4B, 0x68, 0x01, 0xBC, 0x02, 0xBC, 0x02, 0x68, 0x01, 0x1E, 0xB8, 0x0B}; /*{OK,V_W,V_H,H_W,H_H,FPS,bps}*/
    //uint8_t  message[2] = {0x4F, 0x4B}; //return OK to APP
    uint8_t* recvBuf = (uint8_t*) malloc(RECV_BUF_LEN);
    uint8_t* nalBuf = (uint8_t*) malloc(NAL_BUF_LEN);
    uint32_t recv_size = 0, head_nal_size = 0, recv_nal_size = 0, ori_w = 0, ori_h = 0, enc_w = 0, enc_h = 0;
    bool     got_nal = false, mirror_start = false;   
    fd_set readfds;
    struct timeval timeout = {0};
    struct sockaddr_in remote_addr;
    int len = sizeof(struct sockaddr_in);
    
    // ffmpeg
    AVCodec        *codec;
    AVCodecContext *c       = NULL;
    AVFrame        *picture = NULL;
    AVDictionary   *dict    = NULL;
    // ffmpeg
    AVPacket       avpkt;
    int            got_picture = 0, decode_len = 0, w = 0;

#ifdef CFG_VIDEO_ENABLE
	//ituFrameFuncInit();
#endif // CFG_VIDEO_ENABLE
    //itv_set_video_window(100, 0, 300, 480);
    itv_set_pb_mode(1);

    avcodec_register_all(); // avcodec_init();
    codec = avcodec_find_decoder(CODEC_ID_H264);
    if (!codec)
    {
        MACRO_ERTN;
    }
    c               = avcodec_alloc_context3(codec); // avcodec_alloc_context();

#ifdef _DEBUG
    c->debug        = FF_DEBUG_PICT_INFO | FF_DEBUG_SKIP | FF_DEBUG_STARTCODE | FF_DEBUG_PTS | FF_DEBUG_MMCO | FF_DEBUG_BUGS | FF_DEBUG_BUFFERS | FF_DEBUG_THREADS;
#endif

    picture         = avcodec_alloc_frame();
    assert(picture);
    avcodec_get_frame_defaults(picture);
    picture->opaque = malloc(sizeof(uint32_t));
    if (codec->capabilities & CODEC_CAP_TRUNCATED)
        c->flags |= CODEC_FLAG_TRUNCATED;

    if (avcodec_open2(c, codec, &dict) /*avcodec_open(c, codec)*/ < 0)
    {
        MACRO_ERTN;
    }
#ifdef _DEBUG
    av_log_set_level(AV_LOG_DEBUG);
#endif

    //memset(recvBuf,'\0',RECV_BUF_LEN);
    //memset(nalBuf,'\0',NAL_BUF_LEN);
    //f = fopen("E:/test111.264", "wb");
    while(network_mirror_start)
    {
        if(itpGetTickDuration(lastTick) > 2000)
        {
            //printf("#########no video input!!!\n");    
            mirror_start = false;
        }
        FD_ZERO(&readfds);
		FD_SET(sockfd,&readfds);
		timeout.tv_sec = 0;
		timeout.tv_usec = 5000;
		select(sockfd+1,&readfds,NULL,NULL,&timeout); 

        memset(recvBuf,'\0',RECV_BUF_LEN);
        if(!mirror_start)
        {
            if(FD_ISSET(sockfd,&readfds))
            {
                recv_size = recvfrom(sockfd, recvBuf, RECV_BUF_LEN, 0, (struct sockaddr*)&remote_addr, (socklen_t *)&len);
                if (recv_size < 0) 
                {
                    printf ("###could not read UDP start!!\n");
                }else
                {
                    printf("###Received data form %s : %d\n", inet_ntoa(remote_addr.sin_addr), htons(remote_addr.sin_port));
                    for(int i = 0; i < recv_size; i++)
                        printf("0x%x\n", recvBuf[i]);
                    printf("\n");

                    //remote_addr.sin_family = AF_INET;
            	    //remote_addr.sin_port = htons(11111);
            	    //remote_addr.sin_addr.s_addr = inet_addr("192.168.43.1");
            	    if(recvBuf[0] == 0xaa && recvBuf[1] == 0xbb && recvBuf[2] == 0xcc && recvBuf[3] == 0x4)
            	    {
            	    ori_w = recvBuf[12] | (recvBuf[13] << 8);
                    ori_h = recvBuf[14] | (recvBuf[15] << 8);
                    enc_w = ori_w / 2;
                    enc_h = ori_h / 2;
                        /***ITE decode only support 16 alignment***/
                        if (enc_w % 16 != 0)
            				enc_w = enc_w - (enc_w % 16);
                        if (enc_h % 16 != 0)
            				enc_h = enc_h - (enc_h % 16);
                        
                    message[2] = enc_w & 0xFF;
                    message[3] = (enc_w & 0xFF00) >> 8;
                    message[4] = enc_h & 0xFF;
                    message[5] = (enc_h & 0xFF00) >> 8;
                    message[6] = enc_h & 0xFF;
                    message[7] = (enc_h & 0xFF00) >> 8;
                    message[8] = enc_w & 0xFF;
                    message[9] = (enc_w & 0xFF00) >> 8;
                    if (sendto(sockfd, message, sizeof(message), 0, (struct sockaddr*)&remote_addr, len) < 0) 
                    {
                        printf("Could not send OK!!\n");
                    }
                    mirror_start = true;
                    recv_size = 0;
                    lastTick = itpGetTickCount();
                    printf("====>network_mirror_start\n");
                }
                    else if(recvBuf[0] == 0xaa && recvBuf[1] == 0xbb && recvBuf[2] == 0xcc && (recvBuf[3] == 0x0 || recvBuf[3] == 0x6))
                    {
                        mirror_start = true;
                        recv_size = 0;
                        lastTick = itpGetTickCount();
                    }
                }
            }
            continue;
        }
		if(FD_ISSET(sockfd,&readfds))
        {
            recv_size = recv(sockfd, recvBuf, RECV_BUF_LEN, 0);
            lastTick = itpGetTickCount();
            //printf("====>network_mirror size=%d, data<=========\n", recv_size);
            //for(int i = 0; i<recv_size; i++)
            //{
            //    printf("0x%x,",recvBuf[i]);
            //}
            //printf("\n");
            if(recvBuf[0] == 0xaa && recvBuf[1] == 0xbb && recvBuf[2] == 0xcc && recvBuf[3] == 0x6)
        {
                //printf("====>ORI W_H info =%d, data<=========\n", recv_size);
                //for(int i = 0; i<recv_size; i++)
                //{
                //    printf("0x%x,",recvBuf[i]);
                //}
                printf("\n");
                ori_w = recvBuf[12] | (recvBuf[13] << 8);
                ori_h = recvBuf[14] | (recvBuf[15] << 8);
                if(ori_w > ori_h)
                    mirror_from_H = true;
                else
                    mirror_from_H = false;
                recv_size = 0;
            }
		}
        if(recv_size > 0)
        {
            if(recvBuf[16] == 0x0 && recvBuf[17] == 0x0 && recvBuf[18] == 0x0 && recvBuf[19] == 0x1)
            {
                //if(recvBuf[12] == 0x1)
                //    mirror_from_H = true;
                //else
                //    mirror_from_H = false;
                if(recv_nal_size != 0)
                {
                    printf("XXXXXX1\n");
                    recv_nal_size = 0;
                }
                //NAL size from header
                head_nal_size = (recvBuf[11] << 24) | (recvBuf[10] << 16) | (recvBuf[9] << 8) | recvBuf[8];
                head_nal_size -= 4;
                //printf("head nal time =%d(%x, %x, %x, %x)\n", (recvBuf[15] << 24) | (recvBuf[14] << 16) | (recvBuf[13] << 8) | recvBuf[12], recvBuf[15], recvBuf[14], recvBuf[13], recvBuf[12]);
                //fwrite(recvBuf+13, 1, recv_size-13, f);
                //k++;
                memcpy(nalBuf, recvBuf+16, recv_size-16);
                recv_nal_size+=recv_size-16;
            }else
            {
                if(recv_nal_size != 0)
                {
                    //fwrite(recvBuf, 1, recv_size, f);
                    //k++;
                    memcpy(nalBuf + recv_nal_size, recvBuf, recv_size);
                    recv_nal_size += recv_size;
                }
                else
                    printf("XXXXXX2\n");
            }
            if(recv_nal_size == head_nal_size)
            {
                //printf("====>NAL=%d, data<=========\n", recv_nal_size);
                //for(int i = 0; i<recv_nal_size; i++)
                //{
                 //   printf("0x%x,",nalBuf[i]);
                //}
                //printf("\n");
                
                //fwrite(nalBuf, 1, recv_nal_size, f);
                //k++;
                avpkt.data = nalBuf;
                avpkt.size = recv_nal_size;

                len = avcodec_decode_video2(c, picture, &got_picture, &avpkt);
                if (got_picture)    
                {
                    //printf("#######H264 decode ok\n");
                    display_on_LCD(picture);
                }
                recv_nal_size = 0;
                memset(nalBuf,'\0',NAL_BUF_LEN);
            }
            recv_size = 0;
            //if(k > 3000)
            //{
            //    printf("STOOPPPPPPPPPPPPPPPPPP\n");
            //    fclose(f);
            //    break;
            //}
        }
        
        usleep(1000);
    }

    if(recvBuf)
        free(recvBuf);
    if(nalBuf)
        free(nalBuf);
    //if(sockfd)
    //    close(sockfd);
    // ffmpeg de-initial
    //printf("STOOPPPPPPPPPPPPPPPPPP\n");
    //fclose(f);
    avcodec_close(c);
    av_free(c);
    if (picture->opaque)
        free(picture->opaque);
    av_free(picture);
    itv_set_pb_mode(0);
    
    //mirror_is_init = false;
    //pthread_exit(NULL);
   
    return NULL;
}

void network_mirror_init(void)
{
    if(mirror_is_init)
    {
        //network_mirror_start();
        return 0;
    }
    
    mirror_is_init = true;
    while(backup_recving)
        usleep(1000);
    
    struct sockaddr_in myaddr;
    int reuse = 1;
    
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd == -1){
        printf("Fail to create a socket.\n");
        return 0;
    }

    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse));
    
    //socket link
    memset(&myaddr,'\0',sizeof(struct sockaddr_in));
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = INADDR_ANY;
    myaddr.sin_port = htons(11111);
    
	if( bind (sockfd,(struct sockaddr *)&myaddr, sizeof(struct sockaddr_in)) < 0 ){
		printf("udprecv bind udp  error!\n"); 
        return 0;
	}
	else
		printf("udprecv bind udp ok! \n");

    {
        //mirror_is_init = true;
        network_mirror_start = true;
        window_state = 0;
        pthread_attr_t attr, attr_backup;
	    struct sched_param param;
        pthread_attr_init(&attr);
    	param.sched_priority = sched_get_priority_min(1) + 2;
	    pthread_attr_setschedparam(&attr, &param);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        pthread_create(&recv_thread, &attr, udp_mirror_task, NULL);
        if(!backup_is_init)
        {
            pthread_attr_init(&attr_backup);
            pthread_attr_setdetachstate(&attr_backup, PTHREAD_CREATE_DETACHED);
            pthread_create(&backup_recv_thread, &attr_backup, udp_backup_task, NULL);
        }
        printf("====>network_mirror_init\n\n");
    }
}

void network_mirror_stop(void)
{
    mirror_is_init = false;
    network_mirror_start = false;
}

