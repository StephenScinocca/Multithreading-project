#ifdef QUEUE_H_
#define QUEUE_H_

#include <unistd.h>

struct Node* createNode(int i, int l, int c, char d, pthread_cond_t condThread);
struct Queue* createQueue();
 int isEmpty(struct Queue* queue);
int size(struct Queue* queue);
void enqueue(struct Queue* queue, struct Node* node);
struct Node* dequeue(struct Queue* queue);
void printQueue(struct Queue* queue);

#endif
