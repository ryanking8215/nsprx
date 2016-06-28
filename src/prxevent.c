#include <string.h>

#include "prxevent.h"

struct PRXEvent
{
    PRBool status;
    PRMonitor *mon;
};

PR_IMPLEMENT(PRXEvent *) PRX_CreateEvent(PRBool init_status)
{
    PRXEvent *e = PR_NEW(PRXEvent);
    if (e != NULL) {
        memset(e, 0, sizeof(*e));
        e->mon = PR_NewMonitor();
        e->status = init_status;
    }
    return e;
}


PR_IMPLEMENT(void) PRX_DestroyEvent(PRXEvent *e)
{
    if (e != NULL) {
        PR_DestroyMonitor(e->mon);
    }
    PR_FREEIF(e);
}


PR_IMPLEMENT(PRStatus) PRX_SetEvent(PRXEvent *e)
{
    if (e==NULL) {
        return PR_FAILURE;
    }
    PR_EnterMonitor(e->mon);
    e->status = PR_TRUE;
    PR_NotifyAll(e->mon);
    PR_ExitMonitor(e->mon);
    return PR_SUCCESS;
}

PR_IMPLEMENT(PRStatus) PRX_ResetEvent(PRXEvent *e)
{
    if (e==NULL) {
        return PR_FAILURE;
    }
    PR_EnterMonitor(e->mon);
    e->status = PR_FALSE;
    PR_ExitMonitor(e->mon);
    return PR_SUCCESS;
}

PR_IMPLEMENT(PRStatus) PRX_WaitEvent(PRXEvent *e, PRIntervalTime ticks)
{
    if (e==NULL) {
        return PR_FAILURE;
    }

    PRStatus status;
    PR_EnterMonitor(e->mon);
    PRIntervalTime start = PR_IntervalNow();
    while (!e->status) {
        status = PR_Wait(e->mon, ticks);
        if (status == PR_FAILURE) {
            PR_ExitMonitor(e->mon);
            return status;
        }
        if (PR_IntervalNow()-start > ticks) {
            PR_ExitMonitor(e->mon);
            return PR_FAILURE;
        }
    }

    e->status = PR_FALSE;
    PR_ExitMonitor(e->mon);
    return PR_SUCCESS;
}
