#define _GNU_SOURCE

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sched.h>
#include <unistd.h>

#define CROSSINGS 100000
#define NUM_THREADS 2
int bridge[2];
int accident = 0;

void *bridge_crossing(void *threadid)
{
  long tid;
  int i;
  tid = (long)threadid;
  int cpu = sched_getcpu();
  printf("\n I am Car %ld \non cpu %d \n", tid, cpu);
  for (i = 0; i < CROSSINGS; i++)
  {
    // fahre auf Bruecke
    bridge[0] ++;
    // fahre von Bruecke
    bridge[1] --;
  }
}

int main(int argc, char *argv[])
{
  // initialisiere Daten
  pthread_t threads[NUM_THREADS];
  int rc, i;
  long t;
  bridge[0] = bridge[1] = 0;

  // starte threads
  for (t = 0; t < NUM_THREADS; t++)
  {
    printf("In main: creating thread %ld\n", t);
    rc = pthread_create(&threads[t], NULL, bridge_crossing, (void *)t);
    if (rc)
    {
      printf("ERROR; return code from pthread_create () is %d\n", rc);
      exit(-1);
    }
  }
  // joining threads
  for (t = 0; t < NUM_THREADS; t++)
  {
    pthread_join(threads[t], NULL);
  }

  // Anzahl an Zusammenstoessen
  accident = abs(abs(bridge[0]) - abs(bridge[1]));
  printf("accidents car0: %d \n", accident);

  // exit
  pthread_exit(NULL);
}