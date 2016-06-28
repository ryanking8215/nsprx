#include "nsprx.h"

typedef struct MyItem
{
    int value;
    PRTime time;
} MyItem;

static void DestoryMyItem(void *arg)
{
    PR_FREEIF(arg);
}

struct Test1
{
    PRThread * productor_thread;
    PRThread * comsumer_thread;
    PRXChannel * chan;
};

static void productor_task(void *arg)
{
    struct Test1 *t = (struct Test1 *)arg;
    int i;
    for (i=0; i<10; i++) {
        MyItem *item = PR_NEW(MyItem);
        item->value = i;
        item->time = PR_Now();
        PRX_ChannelPut(t->chan, (void *)item);
        /* PR_Sleep(PR_SecondsToInterval(1)); */
    }
    printf("exit\n");
}

static void comsumer_task(void *arg)
{
    struct Test1 *t = (struct Test1 *)arg;
    MyItem *item;

    while (1) {
        item = (MyItem *)PRX_ChannelGet(t->chan);
        printf("item:%d\n", item->value);
        DestoryMyItem((void *)item);
    }
}

int main(void)
{
    PR_Init(PR_USER_THREAD, PR_PRIORITY_NORMAL, 0);

    struct Test1 t;
    /* t.chan = PRX_CreateChannel(DestoryMyItem); */
    t.chan = PRX_CreateChannelWithMaxSize(1, DestoryMyItem);
    t.comsumer_thread = PRX_CreateUserThread(comsumer_task, &t, PR_JOINABLE_THREAD);
    t.productor_thread = PRX_CreateUserThread(productor_task, &t, PR_JOINABLE_THREAD);

    PR_JoinThread(t.productor_thread);
    printf("productor is over\n");
    PR_JoinThread(t.comsumer_thread);
    printf("consumer is closed");
    PRX_DestroyChannel(t.chan);

    return 0;
}
