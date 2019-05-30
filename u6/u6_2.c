#define _GNU_SOURCE

#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#define NUM_PLACES 6

// struct um der thread Funktion mehr als ein Value zu uebergeben
struct thread_args
{
    long threadid;
    int num_threads;
};

// global variables

struct timespec max_timeout;
int last = 0;
int timestamps[NUM_PLACES] = {0};
int buffer[NUM_PLACES] = {0};

pthread_mutex_t mutex;
pthread_cond_t is_empty;
pthread_cond_t is_full;

void ablegen(void *t_args, int i)
{
    struct thread_args *args = (struct thread_args *)t_args;
    pthread_mutex_lock(&mutex);
    while (last == NUM_PLACES)
    {
        pthread_cond_wait(&is_full, &mutex);
    }
    
    // Gehe durch timestamps um erste leere Stelle zu finden und fuellen
    // alle befuellten Stellen werden inkrementiert
    int written_to_empty = 0;
    for (int j = 0; j < NUM_PLACES; j++)
    {
        if (timestamps[j] != 0)
        {
            timestamps[j]++;
        }
        else if (timestamps[j] == 0 && written_to_empty == 0)
        {
            timestamps[j] = 1;
            buffer[j] = i;
            written_to_empty = 1;
            printf("Producer %ld puts %d into buffer at place %d \n",
                args->threadid,
                buffer[j],
                j);
        }
    }
    last++;
    pthread_cond_signal(&is_empty);
    pthread_mutex_unlock(&mutex);
}

int entnehmen(void *t_args)
{
    struct thread_args *args = (struct thread_args *)t_args;
    pthread_mutex_lock(&mutex);
    while (last == 0)
    {
        if (pthread_cond_timedwait(&is_empty, &mutex, &max_timeout) != 0)
        {
            printf("Consumer %ld Timedout\n", args->threadid);
            pthread_mutex_unlock(&mutex);
            return 1;
        }
    }

    // suche groessten timestamp und seine Position
    int max_timestamp = timestamps[0];
    int max_pos = 0;
    for (int j = 1; j < NUM_PLACES; j++)
    {
        if (timestamps[j] > max_timestamp)
        {
            max_timestamp = timestamps[j];
            max_pos = j;
        }
    }
    timestamps[max_pos] = 0;

    printf("Consumer %ld takes %d from position %d\n",
           args->threadid,
           buffer[max_pos],
           max_pos);
    last--;
    pthread_cond_signal(&is_full);
    pthread_mutex_unlock(&mutex);
    return 0;
}

void *producer(void *t_args)
{
    //struct thread_args *args = (struct thread_args *)t_args;
    for (int i = 0; i < 1000; i++)
    {
        ablegen(t_args, i);
    }
    return 0;
}

void *consumer(void *t_args)
{
    //struct thread_args *args = (struct thread_args *)t_args;
    while (1)
    {
        if (entnehmen(t_args) != 0)
        {
            break;
        }
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

    // init threads
    pthread_t threads_consumer[num_threads_consumers];
    pthread_t threads_producer[num_threads_producers];

    pthread_cond_init(&is_empty, NULL);
    pthread_cond_init(&is_full, NULL);
    //pthread_cond_signal(&is_empty);
    //pthread_cond_signal(&is_full);

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

    // starte producer threads
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

    // joining consumer threads
    for (t = 0; t < num_threads_consumers; t++)
    {
        pthread_join(threads_consumer[t], NULL);
    }
    printf("Closing consumer threads\n");

    // joining producer threads
    for (t = 0; t < num_threads_producers; t++)
    {
        pthread_join(threads_producer[t], NULL);
    }
    printf("Closing producer threads\n");

    // destroy condition variables
    pthread_cond_destroy(&is_empty);
    pthread_cond_destroy(&is_full);

    // exit
    return 0;
}
