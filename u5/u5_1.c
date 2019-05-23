#define _GNU_SOURCE

#include <assert.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sched.h>
#include <unistd.h>
#include <semaphore.h>

#define NUM_PLACES 3

// struct um der thread Funktion mehr als ein Value zu uebergeben
struct thread_args
{
    long threadid;
    int num_threads;
};

int last = 0;

int buffer[NUM_PLACES];

void *producer(void *t_args)
{

    struct thread_args *args = (struct thread_args *)t_args;

    for (int i = 0; i < 1000; i++)
    {
        buffer[last] = i;
        printf("Producer %ld puts %d into buffer at place %d \n",
               args->threadid,
               buffer[last],
               last);
        last++;
    }
}

void *consumer(void *t_args)
{
    struct thread_args *args = (struct thread_args *)t_args;

    while (1)
    {
        printf("Consumer %ld takes %d \n",
               args->threadid,
               buffer[last - 1]);
        last--;
    }
}

int main(int argc, char *argv[])
{
    // initialisiere Daten
    int num_threads_consumers, num_threads_producers;
    int rc, i;
    long t;

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

    // joining threads
    for (t = 0; t < num_threads_producers; t++)
    {
        pthread_join(threads_producer[t], NULL);
    }

    // exit
    return 0;
}
