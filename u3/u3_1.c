#define _GNU_SOURCE

#include <assert.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sched.h>
#include <unistd.h>

#define DEFAULT_NUM_THREADS 2
#define CROSSINGS 100000
int bridge = 0;
int accident = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// struct um der thread Funktion mehr als ein Value zu uebergeben
struct thread_args
{
  long threadid;
  int num_threads;
};

// Funktion fuer die threads
void *bridge_crossing(void *t_args)
{
  struct thread_args *args = (struct thread_args *)t_args;
  long tid;
  int i;
  int _error = 0;
  tid = args->threadid;
  int cpu = sched_getcpu();
  printf("\n I am Car %ld \non cpu %d \n", tid, cpu);
  for (i = 0; i < CROSSINGS; i++)
  {
    _error = pthread_mutex_lock(&mutex);
    // Brueckenueberquerung
    bridge++;
    _error = pthread_mutex_unlock(&mutex);
  }
  pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
  // initialisiere Daten
  int num_threads = DEFAULT_NUM_THREADS;
  int rc, i;
  long t;
  
  // init lock
  pthread_mutex_init(&mutex, NULL);

  // Input
  printf("How Many Cars would you like to release? : \n");
  scanf("%d", &num_threads);
  // handle Input
  if (num_threads > 1)
  {
    ;
  }
  else if (num_threads == 1)
  {
    printf("One singel Car wont have issues");
    exit(0);
  }
  else
  {
    printf("We do not do that stuff here");
    exit(0);
  }

  // init threads
  pthread_t threads[num_threads];

  // starte threads
  for (t = 0; t < num_threads; t++)
  {
    struct thread_args *t_args = malloc(sizeof(struct thread_args));
    t_args->threadid = t;
    t_args->num_threads = num_threads;
    // printf("In main: creating thread %ld\n", t);
    rc = pthread_create(&threads[t], NULL, bridge_crossing, t_args);
    if (rc)
    {
      printf("ERROR; return code from pthread_create () is %d\n", rc);
      exit(-1);
    }
  }
  // joining threads
  for (t = 0; t < num_threads; t++)
  {
    pthread_join(threads[t], NULL);
  }

  // Anzahl an Zusammenstoessen
  printf("bridge: %d\n", bridge);
  // Unfaelle gleich die Anzahl der angeordneten Ueberquerungen
  // minus die Anzahl der tatsaechlichen Ueberquerungen
  accident = (CROSSINGS * num_threads) - bridge;
  printf("accidents: %d \n", accident);

  // exit
  pthread_exit(NULL);
}