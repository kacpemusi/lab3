#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define MAX_QUEUE_SIZE 50

typedef struct queue
{
    int items[MAX_QUEUE_SIZE];
    int front;
    int rear;
    int count;
    int id;
    sem_t busy;
    sem_t full;
    sem_t empty;
} queue;

void* producer(void*);
void* consumer(void*);
queue q1;
queue q2;
queue q3;
queue q4;

void init_queue(queue *q, int id)
{
    q->front = 0;
    q->rear = -1;
    q->count = 0;
    q->id =  id;
    sem_init(&q->busy, 0, 1);
    sem_init(&q->full, 0, 0);
    sem_init(&q->empty, 0, MAX_QUEUE_SIZE);
}

void insert(queue *q, int item, int isSpecial)
{
    printf("\n");
    printf("S%d empty down\n", q->id);
    sem_wait(&q->empty);
    printf("S%d busy down\n", q->id);
    sem_wait(&q->busy);
    q->rear = (q->rear + 1) % MAX_QUEUE_SIZE;
    q->items[q->rear] = item;
    q->count++;
    sem_post(&q->busy);
    printf("S%d busy up\n", q->id);
    sem_post(&q->full);
    printf("S%d full up\n", q->id);
    if(isSpecial)
        sleep(2);
}


int pop(queue *q, int isSpecial)
{
    printf("\n");
    if(!isSpecial){
    printf("S%d full down\n", q->id);
    sem_wait(&q->full);}
    printf("S%d busy down\n", q->id);
    sem_wait(&q->busy);
    int item = q->items[q->front];
    q->front = (q->front + 1) % MAX_QUEUE_SIZE;
    q->count--;
    sem_post(&q->busy);
    printf("S%d busy up\n", q->id);
    sem_post(&q->empty);
    printf("S%d empty up\n", q->id);
    return item;
}

void* producer(void *p)
{
    queue *q = (queue*)p;
    int isSpecial = q->id == 4;
    int item;
    while (1)
    {
        item = rand()%2+1;
        sleep(item);
        insert(q, item, isSpecial);
        printf("Q%d PROD item %d\n", q->id, item);
    }
}

void* consumer(void *p) {
    queue *q = (queue*)p;
    int item;
    while (1) {
        item = rand()%2+1;
        sleep(item);
        if (sem_trywait(&q4.full) == 0) {
            item = pop(&q4, 1);
            printf("Consumed item %d from special buffer\n", item);
        } else {
            item = pop(q,0);
            printf("Q%d CONS item %d\n", q->id, item);
        }
    }
}

int main(int argc, char* argv[])
{
    init_queue(&q1,1);
    init_queue(&q2,2);
    init_queue(&q3,3);
    init_queue(&q4,4);

    pthread_t prod1, prod2, prod3, prod4, cons1, cons2, cons3;
    pthread_create(&prod1, NULL, producer, &q1);
    pthread_create(&prod2, NULL, producer, &q2);
    pthread_create(&prod3, NULL, producer, &q3);
    pthread_create(&prod4, NULL, producer, &q4);

    pthread_create(&cons1, NULL, consumer, &q1);
    pthread_create(&cons2, NULL, consumer, &q2);
    pthread_create(&cons3, NULL, consumer, &q3);

    pthread_join(prod1, NULL);
    pthread_join(prod2, NULL);
    pthread_join(prod3, NULL);
    pthread_join(prod4, NULL);
    pthread_join(cons1, NULL);
    pthread_join(cons2, NULL);
    pthread_join(cons3, NULL);

    return 0;
}



