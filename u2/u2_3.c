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
char *_lock;

// struct um der thread Funktion mehr als ein Value zu uebergeben
struct thread_args
{
  long threadid;
  int num_threads;
};

// aus der VL
int lock(long tid, int num_threads)
{
  _lock[tid] = 1;
  int i = 0;

  for (i = 0; i < num_threads; i++)
  {
    if (i == tid)
    {
      continue;
    }
    if (_lock[i])
    {
      _lock[tid] = 0;
      sleep(1);
      _lock[tid] = 1;
    }
  }
  return 0;
}

int unlock(long tid)
{
  _lock[tid] = 0;
  return 0;
}

// Funktion fuer die threads
void *bridge_crossing(void *t_args)
{
  struct thread_args *args = (struct thread_args *)t_args;
  long tid;
  int i;
  tid = args->threadid;
  int cpu = sched_getcpu();
  printf("\n I am Car %ld \non cpu %d \n", tid, cpu);
  for (i = 0; i < CROSSINGS; i++)
  {
    lock(tid, args->num_threads);
    // Brueckenueberquerung
    bridge++;
    unlock(tid);
  }
}

int main(int argc, char *argv[])
{
  // initialisiere Daten
  pthread_t threads[DEFAULT_NUM_THREADS];
  int num_threads = DEFAULT_NUM_THREADS;
  int rc, i;
  long t;
  // Input
  printf("How Many Cars would you like to release? : \n");
  scanf("%d", &num_threads);
  // handle Input
  if (num_threads > 1)
  {
    pthread_t threads[num_threads];
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
  // allociere Speicher fuer _lock
  _lock = malloc(sizeof(char) * num_threads);
  assert(_lock);

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