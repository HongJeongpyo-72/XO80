#include <string.h>
#include <malloc.h>
#include "packet_queue.h"

//=============================================================================
//                Constant Definition
//=============================================================================

//=============================================================================
//                Macro Definition
//=============================================================================

//=============================================================================
//                Structure Definition
//=============================================================================

//=============================================================================
//                Global Data Definition
//=============================================================================

//=============================================================================
//                Private Function Definition
//=============================================================================

//=============================================================================
//                Public Function Definition
//=============================================================================

//=============================================================================
/**
 * Initialize packet queue
 *
 * @param q             pointer of packet queue.
 * @param pfPktRelease  release resource callback function for packet queue
 *                      entry.
 * @param maxPackets    max packet entries of the queue
 * @param entrySize     size of each packet queue entry.
 * @return              none
 */
//=============================================================================
void
packetQueueInit(
    PktQueue *q,
    void     *pfPktRelease,
    int      maxPackets,
    int      entrySize)
{
    memset(q, 0, sizeof(PktQueue));
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->cond, NULL);
    q->maxPackets   = maxPackets;
    q->entrySize    = entrySize;
    q->pfPktRelease = pfPktRelease;
}

//=============================================================================
/**
 * Flush packet queue entries
 * @param q             pointer of packet queue.
 * @return              none
 */
//=============================================================================
void
packetQueueFlush(
    PktQueue *q)
{
    QueuePktList *pkt, *pkt1;
    if (q->mutex)
    {
        pthread_mutex_lock(&q->mutex);
        for (pkt = q->firstPkt; pkt != NULL; pkt = pkt1)
        {
            pkt1 = pkt->next;
            if (q->pfPktRelease)
            {
                q->pfPktRelease(pkt->pkt);
            }
            free(pkt);
        }
        q->lastPkt    = NULL;
        q->firstPkt   = NULL;
        q->numPackets = 0;
        pthread_mutex_unlock(&q->mutex);
    }
}

//=============================================================================
/**
 * Terminate packet queue.
 * @param q             pointer of packet queue.
 * @return              none
 */
//=============================================================================
void
packetQueueEnd(
    PktQueue *q)
{
    if (q->mutex)
    {
        packetQueueFlush(q);
        pthread_mutex_destroy(&q->mutex);
        pthread_cond_destroy(&q->cond);
        memset(q, 0, sizeof(PktQueue));
    }
}

//=============================================================================
/**
 * Put entry to packet queue
 *
 * @param q             pointer of packet queue.
 * @param pkt           input packet queue entry.
 * @return              0 - success. otherwise - failed
 */
//=============================================================================
int
packetQueuePut(
    PktQueue *q,
    void     *pkt)
{
    QueuePktList *pkt1;

    /* duplicate the packet */
    if (q->mutex)
    {
        pkt1      = malloc(sizeof(QueuePktList));
        if (!pkt1) return -1;

        pkt1->pkt = malloc(q->entrySize);
        if (pkt1->pkt && pkt)
        {
            memcpy(pkt1->pkt, pkt, q->entrySize);
        }
        else
        {
            if (pkt1->pkt) free(pkt1->pkt);
            if (pkt1) free(pkt1);
            return 0;
        }
        pkt1->next = NULL;

        pthread_mutex_lock(&q->mutex);

        if (!q->lastPkt)
            q->firstPkt = pkt1;
        else
            q->lastPkt->next = pkt1;
        q->lastPkt = pkt1;
        q->numPackets++;

        //remove the first packet(oldest) once packet queue is full.
        if (q->numPackets > q->maxPackets)
        {
            pkt1        = q->firstPkt;
            q->firstPkt = pkt1->next;
            if (q->pfPktRelease)
            {
                q->pfPktRelease(pkt1->pkt);
            }
            free(pkt1);
            q->numPackets--;
        }

        /* XXX: should duplicate packet data in DV case */
        pthread_cond_signal(&q->cond);
        pthread_mutex_unlock(&q->mutex);
        return 0;
    }
    else
    {
        return -1;
    }
}

//=============================================================================
/**
 * Retrieve the first entry of packet queue
 *
 * @param q             pointer of packet queue.
 * @param pkt           output first packet queue entry.
 * @param block         1 - wait till any entry been put in queue. 0 - no block
 * @return              0 - no entry. 1 - get packet entry. -1 - error occured
 */
//=============================================================================
int
packetQueueGet(
    PktQueue *q,
    void     **pkt,
    int      block)
{
    QueuePktList *pkt1;
    int          ret;
    if (q->mutex)
    {
        pthread_mutex_lock(&q->mutex);

        for (;;)
        {
            pkt1 = q->firstPkt;
            if (pkt1)
            {
                q->firstPkt = pkt1->next;
                if (!q->firstPkt)
                    q->lastPkt = NULL;
                q->numPackets--;
                *pkt = pkt1->pkt;
                free(pkt1);
                ret  = 1;
                break;
            }
            else if (!block)
            {
                ret = 0;
                break;
            }
            else
            {
                pthread_cond_wait(&q->cond, &q->mutex);
            }
        }
        pthread_mutex_unlock(&q->mutex);
        return ret;
    }
    else
    {
        return -1;
    }
}