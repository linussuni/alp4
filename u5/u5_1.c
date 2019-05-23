#define _GNU_SOURCE

#include <assert.h>
#include <errno.h>
#include <pthread.h>
//#include <sched.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>

#define NUM_PLACES 6

// struct um der thread Funktion mehr als ein Value zu uebergeben
struct thread_args
{
    long threadid;
    int num_threads;
};

struct timespec max_timeout;
int last = 0;
int buffer[NUM_PLACES];

// semaphores

sem_t mutex;
sem_t empty;
sem_t full;

void *producer(void *t_args)
{
    struct thread_args *args = (struct thread_args *)t_args;

    for (int i = 0; i < 1000; i++)
    {
        sem_wait(&full);
        sem_wait(&mutex);
        buffer[last] = i;
        printf("Producer %ld puts %d into buffer at place %d \n",
               args->threadid,
               buffer[last],
               last);
        last++;
        sem_post(&mutex);
        sem_post(&empty);
    }
    return 0;
}

void *consumer(void *t_args)
{
    struct thread_args *args = (struct thread_args *)t_args;
    int s;

    while (1)
    {
        s = sem_timedwait(&empty, &max_timeout);
        if(s != 0)
        {
            printf("No products to comsume - > timedout\n");
            break;
        }
        //sem_wait(&empty);
        sem_wait(&mutex);
        printf("Consumer %ld takes %d \n",
               args->threadid,
               buffer[last - 1]);
        last--;
        sem_post(&mutex);
        sem_post(&full);
    }
    return 0;
}

int main(int argc, char *argv[])
{
    // initialisiere Daten
    int num_threads_consumers, num_threads_producers;
    int rc;
    long t;

    clock_gettime(CLOCK_REALTIME, &max_timeout);
    max_timeout.tv_sec += 5;

    // Input
    printf("How many consumers? : ");
    scanf("%d", &num_threads_consumers);

    printf("How many producers? : ");
    scanf("%d", &num_threads_producers);
    // handle Input
    if (num_threads_consumers > 0 || num_threads_producers > 0)
    {
        ;
    }
    else
    {
        printf("\nERROR: wrong input\n");
        exit(0);
    }
    printf("Number of consumers: >%d< \n", num_threads_consumers);
    printf("Number of producers: >%d< \n", num_threads_producers);

    // init threads & buffer
    pthread_t threads_consumer[num_threads_consumers];
    pthread_t threads_producer[num_threads_producers];
    for (t = 0; t < NUM_PLACES; t++)
    {
        buffer[t] = 0;
    }

    // init semaphores
    sem_init(&mutex, 0, 1);
    sem_init(&empty, 0, 0);
    sem_init(&full, 0, NUM_PLACES);

    // starte consumer threads
    for (t = 0; t < num_threads_consumers; t++)
    {
        struct thread_args *t_args = malloc(sizeof(struct thread_args));
        t_args->threadid = t;
        t_args->num_threads = num_threads_consumers;

        // producer
        rc = pthread_create(&threads_consumer[t], NULL, consumer, t_args);
        if (rc)
        {
            printf("ERROR: return code from pthread_create () is %d\n", rc);
            exit(-1);
        }
    }

    // starte consumer threads
    for (t = 0; t < num_threads_producers; t++)
    {
        struct thread_args *t_args = malloc(sizeof(struct thread_args));
        t_args->threadid = t;
        t_args->num_threads = num_threads_producers;

        // producer
        rc = pthread_create(&threads_producer[t], NULL, producer, t_args);
        if (rc)
        {
            printf("ERROR: return code from pthread_create () is %d\n", rc);
            exit(-1);
        }
    }

    // joining threads
    for (t = 0; t < num_threads_consumers; t++)
    {
        pthread_join(threads_consumer[t], NULL);
    }
    printf("Closing consumer threads\n");

    // joining threads
    for (t = 0; t < num_threads_producers; t++)
    {
        pthread_join(threads_producer[t], NULL);
    }
    printf("Closing producer threads\n");

    sem_destroy(&mutex);
    sem_destroy(&full);
    sem_destroy(&empty);

    // exit
    return 0;
}
