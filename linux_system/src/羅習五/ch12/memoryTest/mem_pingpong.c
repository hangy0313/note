#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

/*a和b大小只有4個byte，二者合計8個byte*/
struct DS {
    volatile long a;
    volatile long b;
};

struct DS ds;

void* thread(void* local)
{
    int select = (int) local;
    long long i;

    for (i = 0; i < 10000000000L; i++) {
        if (select == 1) {
            ds.a++;
        } else {
            ds.b++;
        }
    }

    if (select == 1) {
        printf("%ld\n", ds.a);
    } else {
        printf("%ld\n", ds.b);
    }
}

int main(void)
{
    pthread_t id1, id2;
    volatile struct DS ds;
    ds.a = 0;
    ds.b = 0;
    printf("sizeof(DS)=%d\n", (int)sizeof(ds));
    pthread_create(&id1, NULL, thread, (void*) 0);
    pthread_create(&id2, NULL, thread, (void*) 1);
    pthread_join(id1, NULL);
    pthread_join(id2, NULL);
    return (0);
}
