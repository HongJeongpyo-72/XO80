#include "flower/flower.h"
typedef mblk_ite rbuf_ite;

typedef struct _mblkq
{
    mblk_ite qstop;
    int qcount;
    rbuf_ite *rb;
} mblkq;

/*ring buff*/
rbuf_ite *ite_rbuf_init(size_t size);
void ite_rbuf_free(rbuf_ite *m);
unsigned int ite_rbuf_get_avail_size(rbuf_ite *m);
int ite_rbuf_put(rbuf_ite *mp,char *src,int sample);
int ite_rbuf_get(char *dst,rbuf_ite *mp,int sample);
void ite_rbuf_flush(rbuf_ite *mp);

/*link list*/
void mblkqinit(mblkq *q);
void putmblkq(mblkq *q, mblk_ite *m);
mblk_ite *getmblkq(mblkq *q);
void flushmblkq(mblkq *q);

/*shape mblkQ*/
void mblkQShapeInit(mblkq *q,int bufsize);
void mblkQShapeUninit(mblkq *q);
void mblkQShapePut(mblkq *q,mblk_ite *m,int resize);
void ite_queue_put_from_mblkQ(IteQueueblk *blk,IteFilter *f,int pin,mblkq *tmpQ);

/*Qnumber controller*/
int IteAudioQueueController(IteFilter *f,int pin,int max,int min);