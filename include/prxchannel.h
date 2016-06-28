#ifndef prxchannel_h__
#define prxchannel_h__

#include <nspr/nspr.h>

PR_BEGIN_EXTERN_C

typedef struct PRXChannel PRXChannel;
typedef void (PR_CALLBACK *PRXChannelItemDtor)(void *data);

NSPR_API(PRXChannel *) PRX_CreateChannel(PRXChannelItemDtor dtor);
NSPR_API(PRXChannel *) PRX_CreateChannelWithMaxSize(PRUint32 maxsize, PRXChannelItemDtor dtor);

NSPR_API(PRBool) PRX_GetChannelEmpty(PRXChannel *);
NSPR_API(PRBool) PRX_GetChannelFull(PRXChannel *);
NSPR_API(PRUint32) PRX_GetChannelSize(PRXChannel *);

NSPR_API(void) PRX_DestroyChannel(PRXChannel *);

NSPR_API(PRStatus) PRX_ChannelPutNoWait(PRXChannel *chan, void *data);
NSPR_API(PRStatus) PRX_ChannelPut(PRXChannel *chan, void *data);
NSPR_API(PRStatus) PRX_ChannelPutFull(PRXChannel *chan, void *data, PRIntervalTime ticks);

NSPR_API(void *) PRX_ChannelGet(PRXChannel *chan);
NSPR_API(void *) PRX_ChannelGetFull(PRXChannel *chan, PRIntervalTime ticks);

PR_END_EXTERN_C

#endif
