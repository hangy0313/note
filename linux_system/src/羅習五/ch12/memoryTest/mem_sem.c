#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdatomic.h>

int global = 0;
sem_t semaphores;
void thread(void* local)
{
    unsigned long long i;

    for (i = 0; i < 100000000; i++) {
        sem_wait(&semaphores);  //向OS要求進入critical section修改global
        global += 1;
        sem_post(&semaphores);  //告訴OS修改完成，離開critical section
    }

    printf("%d\n", global);
}

int main(void)
{
    pthread_t id1, id2;
    sem_init(&semaphores, 0,
             1);    /*0: thread使用，1: semaphore只允許一個人修改global*/
    printf("sizeof(DS)=%d\n", (int)sizeof(atomic_int));
    pthread_create(&id1, NULL, (void*) thread, NULL);
    pthread_create(&id2, NULL, (void*) thread, NULL);
    pthread_join(id1, NULL);
    pthread_join(id2, NULL);
    return (0);
}
