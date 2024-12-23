#ifndef _PACKET_QUEUE_H__
#define _PACKET_QUEUE_H__

#include "pthread.h"

#ifdef __cplusplus
extern C {
#endif

//=============================================================================
//                Constant Definition
//=============================================================================

//=============================================================================
//                Macro Definition
//=============================================================================

typedef void (*pfPktReleaseCb) (void *pkt);

//=============================================================================
//                Structure Definition
//=============================================================================

typedef struct QueuePktList {
    void* pkt;
    struct QueuePktList *next;
} QueuePktList;

typedef struct PktQueue {
    QueuePktList *firstPkt, *lastPkt;
    int numPackets;
    int maxPackets;
    int entrySize;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    pfPktReleaseCb pfPktRelease;
} PktQueue;

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
 * @param pfPktRelease  release resource cb for packet queue entry.
 * @param maxPackets    max packet entries of the queue
 * @param entrySize     size of each packet queue entry.
 * @return              none
 */
//=============================================================================
void
packetQueueInit(
    PktQueue *q,
    void* pfPktRelease,
    int maxPackets,
    int entrySize);

//=============================================================================
/**
 * Flush packet queue entries
 * @param q             pointer of packet queue.
 * @return              none
 */
//=============================================================================
void
packetQueueFlush(
    PktQueue *q);

//=============================================================================
/**
 * Terminate packet queue.
 * @param q             pointer of packet queue.
 * @return              none
 */
//=============================================================================
void
packetQueueEnd(
    PktQueue *q);


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
    void *pkt);


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
    void **pkt,
    int block);

#ifdef __cplusplus
}
#endif

#endif

