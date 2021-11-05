#include <pthread.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <cstdio>
#include <stdint.h>

void * thread_function1(void *arg)
{
  const pid_t tid = syscall(SYS_gettid);
  uint32_t count = 0;

  int ret = setpriority(PRIO_PROCESS, tid, -10);
  printf("tid of high priority thread %d , %d\n", tid ,getpriority(PRIO_PROCESS, tid));
  while(1)
  {
    count++;
    printf("high priority ................[%d]\n", count);
    if (count % 1000 == 0)
    {
        sleep(1);
    }
  }
}


void * thread_function(void *arg)
{
  const pid_t tid = syscall(SYS_gettid);
  uint32_t count = 0;

  int ret = setpriority(PRIO_PROCESS, tid, 10);
  printf("tid of low priority thread %d , %d \n", tid ,getpriority(PRIO_PROCESS, tid));
  while(1)
  {
    count++;
    printf("lower priority...[%d]\n", count);
    if (count % 1000 == 0)
    {
        sleep(1);
    }
  }
}


int main()
{
  pthread_t id1;
  pthread_t id2;

  pid_t pid = getpid();
  pid_t tid = syscall(SYS_gettid);

  printf("main thread : pid = %d , tid = %d \n" , pid, tid);
  pthread_create(&id1, NULL, thread_function1,  NULL);
  pthread_create(&id2, NULL,thread_function,   NULL);

  pthread_join(id1, NULL);
  pthread_join(id2, NULL);
}
