#include <string.h>

#include "prxchannel.h"


typedef struct Entry
{
    PRCList link;
    void *data;
} Entry;

struct PRXChannel
{
    PRUint32 maxsize;
    PRUint32 size;
    PRCList entries;
    PRMonitor *entry_mon;
    PRXChannelItemDtor dtor;
};

static PRBool _is_empty(PRXChannel *chan)
{
    return chan->size == 0 ? PR_TRUE : PR_FALSE;
}

static PRBool _is_full(PRXChannel *chan)
{
    if (chan->maxsize>0 && chan->size>=chan->maxsize) {
        return PR_TRUE;
    }
    return PR_FALSE;
}

static PRStatus _append(PRXChannel *chan, void *data)
{
    Entry *e = PR_NEW(Entry);
    if (e == NULL) {
        return PR_FAILURE;
    }
    e->data = data;
    PR_APPEND_LINK(&e->link, &chan->entries);
    chan->size+=1;
    return PR_SUCCESS;
}

static void * _pop(PRXChannel *chan)
{
    void *data = NULL;

    Entry *e;
    e = (Entry *)PR_LIST_HEAD(&chan->entries);
    if (e!=NULL) {
        PR_REMOVE_AND_INIT_LINK(&e->link);
        data = e->data;
        PR_FREEIF(e);
        chan->size-=1;
    }
    return data;
}



PR_IMPLEMENT(PRXChannel *) PRX_CreateChannel(PRXChannelItemDtor dtor)
{
    return PRX_CreateChannelWithMaxSize(0, dtor);
}

PR_IMPLEMENT(PRXChannel *) PRX_CreateChannelWithMaxSize(PRUint32 maxsize, PRXChannelItemDtor dtor)
{
    PRXChannel *c = PR_NEW(PRXChannel);
    if (c!=NULL) {
        memset(c, 0, sizeof(*c));
        c->maxsize = maxsize;
        c->dtor = dtor;
        c->entry_mon = PR_NewMonitor();
        PR_INIT_CLIST(&c->entries);
    }
    return c;
}

PR_IMPLEMENT(void) PRX_DestroyChannel(PRXChannel * chan)
{
    Entry *e;
    if (chan==NULL) {
        return;
    }

    PR_EnterMonitor(chan->entry_mon);
    while (!PR_CLIST_IS_EMPTY(&chan->entries)) {
        e = (Entry *)PR_LIST_HEAD(&chan->entries);
        if (e!=NULL) {
            PR_REMOVE_AND_INIT_LINK(&e->link);
            if (chan->dtor != NULL) {
                chan->dtor(e->data);
            }
        }
        PR_FREEIF(e);
    }
    PR_ExitMonitor(chan->entry_mon);

    PR_DestroyMonitor(chan->entry_mon);
    PR_FREEIF(chan);
}

PR_IMPLEMENT(PRBool) PRX_GetChannelEmpty(PRXChannel *chan)
{
    PRBool ret;
    PR_EnterMonitor(chan->entry_mon);
    ret = _is_empty(chan);
    PR_ExitMonitor(chan->entry_mon);
    return ret;
}

PR_IMPLEMENT(PRBool) PRX_GetChannelFull(PRXChannel *chan)
{
    PRBool ret;
    PR_EnterMonitor(chan->entry_mon);
    ret = _is_full(chan);
    PR_ExitMonitor(chan->entry_mon);
    return ret;
}

PR_IMPLEMENT(PRUint32) PRX_GetChannelSize(PRXChannel *chan)
{
    PRUint32 ret;
    PR_EnterMonitor(chan->entry_mon);
    ret = chan->size;
    PR_ExitMonitor(chan->entry_mon);
    return ret;
}

PR_IMPLEMENT(PRStatus) PRX_ChannelPutNoWait(PRXChannel *chan, void *data)
{
    return PRX_ChannelPutFull(chan,data,PR_INTERVAL_NO_WAIT);
}

PR_IMPLEMENT(PRStatus) PRX_ChannelPut(PRXChannel *chan, void *data)
{
    return PRX_ChannelPutFull(chan,data,PR_INTERVAL_NO_TIMEOUT);
}

PR_IMPLEMENT(PRStatus) PRX_ChannelPutFull(PRXChannel *chan, void *data, PRIntervalTime ticks)
{
    if (chan == NULL) {
        return PR_FAILURE;
    }

    PRStatus status;

    PR_EnterMonitor(chan->entry_mon);

    if (ticks == PR_INTERVAL_NO_WAIT) {
        if (_is_full(chan)) {
            PR_ExitMonitor(chan->entry_mon);
            status = PR_FAILURE;
        }
        else {
            status = _append(chan, data);
            PR_NotifyAll(chan->entry_mon);
            PR_ExitMonitor(chan->entry_mon);
        }
        return status;
    }

    PRIntervalTime start = PR_IntervalNow();
    while (_is_full(chan)) {
        status = PR_Wait(chan->entry_mon, ticks);
        if (status == PR_FAILURE) {
            PR_ExitMonitor(chan->entry_mon);
            return PR_FAILURE;
        }
        if (ticks != PR_INTERVAL_NO_TIMEOUT) {
            if (PR_IntervalNow()-start > ticks) {
                PR_ExitMonitor(chan->entry_mon);
                return PR_FAILURE;
            }
        }
    }
    status = _append(chan, data);
    PR_NotifyAll(chan->entry_mon);
    PR_ExitMonitor(chan->entry_mon);
    return PR_SUCCESS;
}

PR_IMPLEMENT(void *) PRX_ChannelGet(PRXChannel *chan)
{
    return PRX_ChannelGetFull(chan, PR_INTERVAL_NO_TIMEOUT);
}

PR_IMPLEMENT(void *) PRX_ChannelGetFull(PRXChannel *chan, PRIntervalTime ticks)
{
    if (chan == NULL) {
        return NULL;
    }

    PRStatus status;
    void *data;
    PR_EnterMonitor(chan->entry_mon);
    if (ticks == PR_INTERVAL_NO_WAIT) {
        if (_is_empty(chan)) {
            data = NULL;
        }
        else {
            data = _pop(chan);
        }
        PR_ExitMonitor(chan->entry_mon);
        return data;
    }

    PRIntervalTime start = PR_IntervalNow();
    while (_is_empty(chan)) {
        status = PR_Wait(chan->entry_mon, ticks);
        if (status == PR_FAILURE) {
            PR_ExitMonitor(chan->entry_mon);
            return NULL;
        }
        if (ticks != PR_INTERVAL_NO_TIMEOUT) {
            if (PR_IntervalNow()-start > ticks) {
                PR_ExitMonitor(chan->entry_mon);
                return NULL;
            }
        }
    }
    data = _pop(chan);
    PR_NotifyAll(chan->entry_mon);
    PR_ExitMonitor(chan->entry_mon);
    return data;
}
