#ifndef prxevent_h__
#define prxevent_h__

#include <nspr/nspr.h>

PR_BEGIN_EXTERN_C

typedef struct PRXEvent PRXEvent;

NSPR_API(PRXEvent *) PRX_CreateEvent(PRBool init_status);
NSPR_API(void) PRX_DestroyEvent(PRXEvent *e);

NSPR_API(PRStatus) PRX_SetEvent(PRXEvent *e);
NSPR_API(PRStatus) PRX_ResetEvent(PRXEvent *e);
NSPR_API(PRStatus) PRX_WaitEvent(PRXEvent *e, PRIntervalTime ticks);


PR_END_EXTERN_C

#endif
