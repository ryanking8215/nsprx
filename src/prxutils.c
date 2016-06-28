#include "prxutils.h"

PR_IMPLEMENT(PRThread *) PRX_CreateUserThread(void (*start)(void *arg), void *arg, PRThreadState state)
{
    return PR_CreateThread(
            PR_USER_THREAD,
            start,
            arg,
            PR_PRIORITY_NORMAL,
            PR_LOCAL_THREAD,
            state,
            0
            );
}


