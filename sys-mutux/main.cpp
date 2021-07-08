#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

// Declaration of thread condition variable
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;

// declaring mutex
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

int done = 1;

void* tick(void*)
{
    for (int i = 0; i < 20; i++)
    {
        printf(".\n");
        sleep(1);
    }
}

// Thread function
void* foo(void*args)
{
    const char* name = reinterpret_cast<char*>(args);
    // acquire a lock
    pthread_mutex_lock(&lock);
    printf("[%s]Locked\n", name);
    if (done == 1) {
        // let's wait on conition variable cond1
        done = 2;
        printf("[%s]Waiting on condition variable cond1\n", name);
        pthread_cond_wait(&cond1, &lock);
        printf("[%s]Received on condition variable cond1\n", name);
    }
    else {
        // Let's signal condition variable cond1
        pthread_cond_signal(&cond1);
        printf("[%s]Signal condition variable cond1\n", name);

        printf("[%s]Start sleeping 5s\n", name);
        sleep(5);
        printf("[%s]Sleeped 5s\n", name);
    }

    // release lock
    printf("[%s]unlock\n", name);
    pthread_mutex_unlock(&lock);

    printf("[%s]Start sleeping 3s\n", name);
    sleep(3);
    printf("[%s]Sleeped 3s\n", name);

    printf("[%s]Returning thread\n", name);

    return NULL;
}

// Driver code
int main()
{
    pthread_t tidt;
    pthread_t tid1, tid2;
    const char* t1_name = "thread1";
    const char* t2_name = "thread2";

    pthread_create(&tidt, NULL, tick, NULL);

    // Create thread 1
    printf("[main]Starting thread1\n");
    pthread_create(&tid1, NULL, foo, const_cast<char*>(t1_name));

    // sleep for 1 sec so that thread 1
    // would get a chance to run first
    sleep(2);

    // Create thread 2
    printf("[main]Starting thread2\n");
    pthread_create(&tid2, NULL, foo, const_cast<char*>(t2_name));

    // wait for the completion of thread 2
    pthread_join(tid2, NULL);

    return 0;
}
