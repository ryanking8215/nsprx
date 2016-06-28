#ifndef prxutils_h__
#define prxutils_h__

#include <nspr/nspr.h>

PR_BEGIN_EXTERN_C

NSPR_API(PRThread *) PRX_CreateUserThread(void (*start)(void *arg), void *arg, PRThreadState state);


PR_END_EXTERN_C

#endif
