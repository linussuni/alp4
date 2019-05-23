#define _GNU_SOURCE

#include <assert.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sched.h>
#include <unistd.h>
#include <semaphore.h>

#define DEFAULT_NUM_THREADS 5

// struct um der thread Funktion mehr als ein Value zu uebergeben
struct thread_args
{
    long threadid;
    int num_threads;
};

sem_t sem_mutex;

// Liste der freien Steabchen
int *staebchen;

// dining philosophers with shared cs
void *philosopher(void *t_args)
{
    struct thread_args *args = (struct thread_args *)t_args;
    long tid = args->threadid;
    int num_threads = args->num_threads;

    printf("\t I am Philosopher %ld and I am . .\n", tid);
    // zwei mal essen
    for (int i = 0; i < 2; i++)
    {
        sem_wait(&sem_mutex);
        while (staebchen[(int)tid] > 0 || staebchen[(int)tid + 1] > 0)
        {
            // nur Staebchen greifen, wenn beide frei sind.
            sleep(1);
        }
        staebchen[(int)tid]++;
        if ((int)tid + 1 == num_threads)
        {
            staebchen[0]++;
        }
        else
        {
            staebchen[(int)tid + 1]++;
        };

        // beide Staebchen greifen
        printf("I am Philosopher %ld and I am currently dining.. \n", tid);

        staebchen[(int)tid]--;
        if ((int)tid + 1 == num_threads)
        {
            staebchen[0]--;
        }
        else
        {
            staebchen[(int)tid + 1]--;
        };

        //beide Staebchen zurueck legen
        printf("I am Philosopher %ld and I just finished dining..\n", tid);
        sem_post(&sem_mutex);
    }
}

int main(int argc, char *argv[])
{
    // initialisiere Daten
    int num_threads = DEFAULT_NUM_THREADS;
    int rc, i;
    long t;

    // Input
    printf("How Many Philosophers would you like to feed? : ");
    scanf("%d", &num_threads);
    // handle Input
    if (num_threads > 1)
    {
        ;
    }
    else if (num_threads == 1)
    {
        printf("\nOne singel Philosoph wont have issues\n");
        exit(0);
    }
    else
    {
        printf("\nWe do not do that stuff here\n");
        exit(0);
    }
    printf("%d \n", num_threads);

    // init threads & steabchen (Betriebsmittel)
    pthread_t threads[num_threads];
    sem_init(&sem_mutex, 0, (num_threads/2)-1);

    // erstelle Array der Laenge num_threads und befuelle es mit null
    staebchen = malloc(sizeof(int) * num_threads);
    for (int i = 0; i < num_threads; i++)
    {
        staebchen[i] = 0;
    }

    // starte threads
    for (t = 0; t < num_threads; t++)
    {
        struct thread_args *t_args = malloc(sizeof(struct thread_args));
        t_args->threadid = t;
        t_args->num_threads = num_threads;
        rc = pthread_create(&threads[t], NULL, philosopher, t_args);
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

    // exit
    sem_destroy(&sem_mutex);

    return 0;
}