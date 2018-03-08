#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <pthread.h>

/**
* The queue structure I found at
* https://www.geeksforgeeks.org/queue-set-1introduction-and-array-implementation/
* although it was an array implementation, and I am using a pointer implementation,
* I used this as a baseline for my code.
*
**/

typedef struct Node {
    int load, cross, id;
    char direction;
    struct Node* next;
    struct Node* prev;
    pthread_cond_t cond;
} Node;

typedef struct Queue {
    struct Node* head;
    struct Node* tail;
    int size;
} Queue;

struct Node* createNode(int i, int l, int c, char d, pthread_cond_t condThread){
    struct Node* node = (struct Node*) malloc(sizeof(struct Node));
    node->id = i;
    node->load = l*100000;
    node->cross = c*100000;
    node->direction = d;
    node->next = NULL;
    node->prev = NULL;
    node-> cond = condThread;
    return node;
}

struct Queue* createQueue() {
    struct Queue* queue = (struct Queue*) malloc(sizeof(struct Queue));
    queue->head = NULL;
    queue->tail = NULL;
    queue->size = 0;
    return queue;
}

struct Node* peek(struct Queue* queue) {
    return queue->head;
}

int isEmpty(struct Queue* queue)
{
    return (queue->size == 0);
}

int size(struct Queue* queue){
    return queue->size;
}

void enqueue(struct Queue* queue, struct Node* node){
    if(queue->size == 0){
        queue->head = node;
        queue->tail = node;
        queue->size++;
    } else {
        queue->size++;
        queue->tail->next = node;
        node->prev = queue->tail;
        queue->tail = node;
        node->next = NULL;
        int flips = 0;
        while (1){

            if (node->load == node->prev->load && node->id < node->prev->id){
            if (queue->size - flips <= 2){
                queue->head->next = node->next;
                queue->head->prev = node;
                if(node->next != NULL){
                    node->next->prev = queue->head;
                }
                node->next = queue->head;
                node->prev = NULL;
                queue->head = node;
                if(flips == 0){
                    queue->tail = node->next;
                }
                break;
            } else {
                struct Node* Last = node->prev;
                struct Node* BeforeLast = Last->prev;
                Last->next = node->next;
                if(node->next != NULL){
                    node->next->prev = Last;
                }
                BeforeLast->next = node;
                node->next = Last;
                Last->prev = node;
                node->prev = BeforeLast;
                if(flips == 0){
                    queue->tail = Last;
                }
            }
        } else {
            break;
        }
        flips++;
    }
}

}

struct Node* dequeue(struct Queue* queue) {
    struct Node* Head = queue->head;
    if(queue->size == 0){
        printf("Queue empty when calling deqeue\n");
        return NULL;
    } else {
        if (queue->size == 1) {
            queue->head = NULL;
            queue->tail = NULL;
        } else {
            queue->head = queue->head->next;
            queue->head->prev = NULL;
        }
        queue->size--;
        //printf("Dequeued\n");
        return Head;
    }
}


void printQueue(struct Queue* queue) {
    printf("Printing what's in the Queue:\n\n");
    struct Node* curr = queue->head;
    while(curr != NULL) {
        printf("Node: %i, l = %d, c = %d, d = %c\n",
            curr->id, curr->load, curr->cross, curr->direction);
        curr = curr->next;
    }
}


/*
int main()
{
    pthread_cond_t pcond;
    struct Queue* queue = createQueue();
    Node* node1 = createNode(1, 2, 3, 'e', pcond);
    Node* node2 = createNode(2, 6, 30,'E', pcond);
    Node* node3 = createNode(3, 6, 7, 'E', pcond);
    Node* node4 = createNode(4, 6, 8, 'e', pcond);
    Node* node5 = createNode(5, 6, 9, 'W', pcond);
    Node* node6 = createNode(6, 6, 10, 'w', pcond);

    enqueue(queue, node1);
    enqueue(queue, node2);
    enqueue(queue, node3);
    enqueue(queue, node6);
    enqueue(queue, node5);
    enqueue(queue, node4);
    printQueue(queue);
    //enqueue(queue, 3,4,'W');
    //enqueue(queue, 10, 25, 'E');
    printf("Size: %d\n", size(queue));
    struct Node* f;
    //printQueue(queue);
    f = dequeue(queue);
    f = dequeue(queue);

    //printQueue(queue);
    return 0;
}
*/
