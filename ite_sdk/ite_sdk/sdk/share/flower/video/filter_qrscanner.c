#include <stdio.h>
#include "flower/flower.h"
#include "flower/fliter_priv_def.h"
#include "ite/ith.h"
#include "qrdecode/quirc.h"
#include "qrdecode/quirc_internal.h"
#include "qrdecode/dbgutil.h"



//=============================================================================
//                              Constant Definition
//=============================================================================
#define QRDECODE_W    450
#define QRDECODE_H    450


static ISPOutPutStream*  gInputStream= NULL;
static int               gCurrentIndex = -1;
static pthread_t         gQRtask;
static bool              gQRthreadRun   = true;  
static bool              gQRScanRun     = false; 
static unsigned char     QRPayload[QRPAYLOAD_MAXSIZE] = {0};
static unsigned short    QRPayloadSize  = 0;
pthread_mutex_t          QRmutex         = PTHREAD_MUTEX_INITIALIZER;

struct result_info {
    int file_count;
    int id_count;
    int decode_count;
};

//=============================================================================
//                              Private Function Declaration
//=============================================================================
//#################################################
//QRCode
//#################################################
static void loadimage(struct quirc *q, unsigned int addr)
{
    unsigned char *src = (unsigned char *)addr;
    unsigned char *dst = q->image;
    unsigned char *Index_Seg, *Index;
    int           SegLenth, IndexLenth;
	int           shift_x = (gInputStream->Width / 2)  - (QRDECODE_W / 2);
	int           shift_y = (gInputStream->Height / 2) - (QRDECODE_H / 2);
    int           start = shift_y, end = shift_y + QRDECODE_H;
    int           i = 0;
    SegLenth   = QRDECODE_W * sizeof(unsigned char);
    IndexLenth = gInputStream->Width * sizeof(unsigned char);
    Index_Seg  = dst;
    Index      = src + ((start * gInputStream->Width) + shift_x) * sizeof(unsigned char);
    for (i = start; i < end; i++)
    {
        memcpy(Index_Seg, Index, QRDECODE_W);
        Index_Seg += SegLenth;
        Index     += IndexLenth;
    }
   
    //printf("dst = %x\n",dst);
}

static int scan_file(struct quirc *q, unsigned int addr, struct result_info *info, struct quirc_data *result)
{
    int          i, j;
    uint8_t      *image;
    unsigned int sfirst_tick = 0;
    unsigned int sdiff_tick  = 0;

    quirc_begin(q, NULL, NULL);

    loadimage(q, addr);

    quirc_end(q);

    if (q->num_capstones < 3)
        return 0;
    
    info->id_count = quirc_count(q);

    for (i = 0; i < info->id_count; i++)
    {
        struct quirc_code code;
        //struct quirc_data data;

        quirc_extract(q, i, &code);

        if (!quirc_decode(&code, result))
        {
            info->decode_count++;
            //ithPrintf("[scanning..]: %5d %5d\n", info->id_count, info->decode_count);
            return 1;
        }
    }

    return 0;
}

static void *DOQRSCAN(void *arg)
{
    struct quirc       *decoder = NULL;
    struct quirc_data  resultdata = {0};
    //struct result_info sum;
    struct result_info info = {0};
    unsigned int         current_addr = 0;
    //QR decode data struct init
    decoder = quirc_new();
    //QR decode size init
    if (quirc_resize(decoder, QRDECODE_W, QRDECODE_H) < 0)
    {
        printf("resize fial\n");
    }
    while (1)
    {
        memset(&resultdata, 0, sizeof(resultdata));
        memset(&info,       0, sizeof(info));

        if (gQRScanRun)
        {
            //scan image content
            pthread_mutex_lock(&QRmutex);
			if(gCurrentIndex > 0)
			{
            	current_addr  = gInputStream->DataAddrY[gCurrentIndex];
			
				pthread_mutex_unlock(&QRmutex);
				//printf("current_addr = %x\n",current_addr);
	            if (scan_file(decoder, current_addr, &info, &resultdata) > 0)
	            {

	                if (resultdata.payload_len > QRPAYLOAD_MAXSIZE)
	                    memcpy((void *)QRPayload, (void *)&resultdata.payload, QRPAYLOAD_MAXSIZE);
	                else
	                    memcpy((void *)QRPayload, (void *)&resultdata.payload, resultdata.payload_len); 

					QRPayloadSize = resultdata.payload_len;
					
	                //printf("payload= %s\n",resultdata.payload);
					//printf("payload len= %d\n",resultdata.payload_len);
					gQRScanRun = false;
	                break;
	            }
			}
			else
				pthread_mutex_unlock(&QRmutex);

        }
        if(gQRthreadRun == false) break;    
        usleep(33 * 1000);
    }

    //destroy QR decode struct
    quirc_destroy(decoder);
    printf("_DOQRSCAN Closed\n");
}

static void qrscanner_init(IteFilter *f)
{
    DEBUG_PRINT("[%s] Filter(%d)\n", __FUNCTION__, f->filterDes.id);

    pthread_attr_t qrattr;
    pthread_attr_init(&qrattr);
    pthread_attr_setstacksize(&qrattr, (size_t)153600);
	gQRthreadRun = true;
	QRPayloadSize = 0;
	gCurrentIndex = -1;
	memset((void *)QRPayload, 0x0, QRPAYLOAD_MAXSIZE);
	pthread_create(&gQRtask, &qrattr, DOQRSCAN, NULL);
}

static void qrscanner_uninit(IteFilter *f)
{
    DEBUG_PRINT("[%s] Filter(%d)\n", __FUNCTION__, f->filterDes.id);
	gQRthreadRun = false;
	pthread_join(gQRtask, NULL);
}

static void qrscanner_process(IteFilter *f)
{
	IteQueueblk 			  rec_blk0 = {0};
	char *dbuf = NULL;

    DEBUG_PRINT("[%s] Filter(%d)\n", __FUNCTION__, f->filterDes.id);

    while(f->run) {
        //printf("[%s] Filter(%d). thread run\n", __FUNCTION__, f->filterDes.id);
        if(ite_queue_get(f->input[0].Qhandle, &rec_blk0) == 0) {
			pthread_mutex_lock(&QRmutex);
			gCurrentIndex = *(unsigned char*)rec_blk0.private1;
	        pthread_mutex_unlock(&QRmutex);
        }				
        usleep(1000);
    }
	

    DEBUG_PRINT("[%s] Filter(%d) end\n", __FUNCTION__, f->filterDes.id);
}

static void f_qrscanner_setinput(IteFilter *f, void *arg)
{
    DEBUG_PRINT("[%s] Filter(%d)\n", __FUNCTION__, f->filterDes.id);
	gInputStream = (ISPOutPutStream*) arg;
	gQRScanRun   = true;
}

static void f_qrscanner_getpayload(IteFilter *f, void *arg)
{
    DEBUG_PRINT("[%s] Filter(%d)\n", __FUNCTION__, f->filterDes.id);
	
	if(QRPayloadSize > 0)
		 memcpy(arg, (void *)QRPayload, QRPAYLOAD_MAXSIZE);

}

static void f_qrscanner_getpayloadsize(IteFilter *f, void *arg)
{
    DEBUG_PRINT("[%s] Filter(%d)\n", __FUNCTION__, f->filterDes.id);
	
	*(unsigned short*)arg = QRPayloadSize;

}



static IteMethodDes FilterQRScanner_methods[] = {
    {ITE_FILTER_QR_SETINPUT, f_qrscanner_setinput},
    {ITE_FILTER_QR_GETPAYLOAD, f_qrscanner_getpayload},
    {ITE_FILTER_QR_GETPAYLOADSIZE, f_qrscanner_getpayloadsize},
    {0, NULL}
};

IteFilterDes FilterQRScanner = {
    ITE_FILTER_QRSCANNER_ID,
    qrscanner_init,
    qrscanner_uninit,
    qrscanner_process,
    FilterQRScanner_methods
};



