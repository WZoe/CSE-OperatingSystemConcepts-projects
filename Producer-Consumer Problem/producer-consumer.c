#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#define BUFFER_SIZE 5
#define RAND_PARAM 100000000

typedef int buffer_item;

buffer_item buffer[BUFFER_SIZE];
pthread_mutex_t mutex;
pthread_t tid;       //Thread ID
sem_t full, empty;
int counter = 0;


int insert_item(buffer_item item)
{
   /* insert item into buffer
    return 0 if successful, otherwise
    return -1 indicating an error condition*/
    if(counter < BUFFER_SIZE)
    {
        buffer[counter] = item;
        counter++;
        return 0;
    }
    else
        return -1;
}


int remove_item(buffer_item *item)
{
   /* insert item into buffer
    return 0 if successful, otherwise
    return -1 indicating an error condition */
    if(counter > 0)
    {
        *item = buffer[(counter-1)];
        counter--;
        return 0;
    }
    else
        return -1;
}


void *producer(void *param)
{
    buffer_item item;

    while(1)
    {
        /* Sleep for a random period of time */
        sleep(rand() / RAND_PARAM);
        /* generate a random number */
        item = rand();

        /* lock */
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);

        /* print info */
        if(insert_item(item))
            fprintf(stderr, "report error condition\n");
        else
            printf("producer produced %d\n", item);

        /* release the mutex lock and signal full */
        pthread_mutex_unlock(&mutex);
        sem_post(&full);
    }
}


void *consumer(void *param)
{
    buffer_item item;

    while(1)
    {
        /* sleep for a random period of time */
        sleep(rand() / RAND_PARAM);

        /* lock */
        sem_wait(&full);
        pthread_mutex_lock(&mutex);

        /* print info */
        if(remove_item(&item))
            fprintf(stderr, "report error condition\n");
        else
            printf("consumer consumed %d\n", item);

        /* release the mutex lock and signal empty */
        pthread_mutex_unlock(&mutex);
        sem_post(&empty);
    }
}


int main(int argc, char *argv[])
{
    int i;

    /* 1. Get command line arguments argv[1], argv[2], argv[3] */
    int SleepTime = atoi(argv[1]);
    int NumofProducers = atoi(argv[2]);
    int NumofConsumers = atoi(argv[3]);

    /* 2. Initialize buffer, semaphores and mutex */
    pthread_mutex_init(&mutex, NULL);
    sem_init(&full, 0, 0);
    sem_init(&empty, 0, BUFFER_SIZE);


    /* 3. Create the producer threads */
    for(i = 0; i < NumofProducers; i++)
        pthread_create(&tid,NULL,producer,NULL);

    /* 4. Create the consumer threads */
    for(i = 0; i < NumofConsumers; i++)
        pthread_create(&tid,NULL,consumer,NULL);


    /* 5. Sleep */
    sleep(SleepTime);

    /* 6. Exit */
    printf("Exiting\n");

    exit(0);
}
