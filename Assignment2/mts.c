#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include "Queue.h"

struct Queue* createQueue();
struct Node* createNode(int i, int l, int c, char d, pthread_cond_t condThread);
struct Node* dequeue(struct Queue* queue);

// Global variables
typedef struct Node {
    int load, cross, id;
    char direction;
    struct Node* next;
    struct Node* prev;
    pthread_cond_t cond;
} Node;

struct timespec start, stop;

char trackDirection;
pthread_mutex_t track;
pthread_mutex_t Mutex_e;
pthread_mutex_t Mutex_E;
pthread_mutex_t Mutex_w;
pthread_mutex_t Mutex_W;
pthread_mutex_t Loop;
pthread_mutex_t Timer;
pthread_mutex_t startThreads;

pthread_cond_t begin;
pthread_cond_t inQ;
pthread_cond_t initial;

struct Queue* e;
struct Queue* w;
struct Queue* E;
struct Queue* W;

// Stops the clock and prints out the time
void getTime(struct timespec* start, struct timespec* stop){
    if( clock_gettime( CLOCK_REALTIME, stop) == -1 ) {
      perror( "clock gettime" );
      exit( EXIT_FAILURE );
    }
    int seconds = 0;
    int hours = 0;
    double accum = ( stop->tv_sec - start->tv_sec ) +
     ( stop->tv_nsec - start->tv_nsec ) / (double)1000000000L;;
     while (accum > 60.0){
         accum = accum - 60.0;
         seconds++;
     }
     while(seconds > 60 ){
         seconds = seconds - 60;
         hours++;
     }
    printf( "%02d:%02d:%04.1f ", hours, seconds, accum );
}

// Function all of the threads run when created
void* threadFunction(void* i){
    // waits till all threads made then signals
    pthread_cond_wait(&begin, &startThreads);
    pthread_mutex_unlock(&startThreads);

    struct Node* node = (struct Node*)i;
    usleep(node->load);
    pthread_mutex_lock(&Timer);
    if(node->direction == 'w' || node->direction == 'W'){
        getTime(&start, &stop);
        printf("Train %2d is ready to go West\n", node->id );
    } else {
        getTime(&start, &stop);
        printf("Train %2d is ready to go East\n", node->id );
    }
    pthread_mutex_unlock(&Timer);
    // After loading, puts trains in correct queue
    if(node->direction == 'e'){
        pthread_mutex_lock(&Mutex_e);
        enqueue(e, node);
        pthread_mutex_unlock(&Mutex_e);
    } else if(node->direction == 'E'){
        pthread_mutex_lock(&Mutex_E);
        enqueue(E, node);
        pthread_mutex_unlock(&Mutex_E);
    } else if(node->direction == 'w'){
        pthread_mutex_lock(&Mutex_w);
        enqueue(w, node);
        pthread_mutex_unlock(&Mutex_w);
    } else if(node->direction == 'W'){
        pthread_mutex_lock(&Mutex_W);
        enqueue(W, node);
        pthread_mutex_unlock(&Mutex_W);
    } else {
        printf("ERROR IN DIRECTION");
    }
    // signals the dispatcher if no trains are in queues that there is one.
    pthread_cond_signal(&initial);
    // waits till dispatcher calls correct train
    pthread_cond_wait(&node->cond, &track);
    pthread_mutex_lock(&Timer);
    if(node->direction == 'w' || node->direction == 'W'){
        getTime(&start, &stop);
        printf("Train %2d is ON the main track going West\n", node->id );
    } else {
        getTime(&start, &stop);
        printf("Train %2d is ON the main track going East\n", node->id );
    }
    pthread_mutex_unlock(&Timer);
    usleep(node->cross);
    pthread_mutex_lock(&Timer);
    if(node->direction == 'w' || node->direction == 'W'){
        getTime(&start, &stop);
        printf("Train %2d is OFF the main track after going West\n", node->id );
    } else {
        getTime(&start, &stop);
        printf("Train %2d is OFF the main track after going East\n", node->id);
    }
    pthread_mutex_unlock(&Timer);
    pthread_mutex_unlock(&track);
}

void loop(int numTrains, pthread_t pthread[]) {
    // waits till a train enters a queue
    pthread_cond_wait(&initial, &Loop);
    while(numTrains > 0) {
        // if trains left remaining but none in the queue,
        //  then this makes the dispatcher wait
        if(isEmpty(e) && isEmpty(E) && isEmpty(w) && isEmpty(W) ) {
            pthread_cond_wait(&initial, &Loop);
        }
        struct Node* n;
        // decides which train is next and calls its condition variable
        if (isEmpty(E) && isEmpty(W)){
            if (isEmpty(w) || (!(isEmpty(e)) && trackDirection == 'W')) {
                trackDirection = 'E';
                 pthread_mutex_lock(&Mutex_e);
                 n = dequeue(e);
                 pthread_mutex_unlock(&Mutex_e);
                 pthread_cond_signal(&n->cond);
             } else if (isEmpty(e) || trackDirection == 'E'){
                 trackDirection = 'W';
                 pthread_mutex_lock(&Mutex_w);
                 n = dequeue(w);
                 pthread_mutex_unlock(&Mutex_w);
                 pthread_cond_signal(&n->cond);
             } else {
                 printf("ERROR\n");
             }
       } else if (isEmpty(W) || (!(isEmpty(E)) && trackDirection == 'W')) {
           trackDirection = 'E';
            pthread_mutex_lock(&Mutex_E);
            n = dequeue(E);
            pthread_mutex_unlock(&Mutex_E);
            pthread_cond_signal(&n->cond);
        } else if (isEmpty(E) || trackDirection == 'E'){
            trackDirection = 'W';
            pthread_mutex_lock(&Mutex_W);
            n = dequeue(W);
            pthread_mutex_unlock(&Mutex_W);
            pthread_cond_signal(&n->cond);
        } else {
            printf("ERROR in dispatchers\n");
        }
        // decrements # of trains when one is finished
        numTrains--;
        // waits to finish the while loop till train
        // is done that is on the track
        pthread_join(pthread[n->id], NULL);
    }
}

int main(int argc, char* argv[]) {
  if(argv[1] == NULL){
      printf("Nothing in argv[1]\n");
      return 0;
  }
  // Initialize everything.
  pthread_cond_init(&inQ, NULL);
  pthread_cond_init(&initial, NULL);
  pthread_mutex_init(&track, NULL);
  pthread_mutex_init(&Mutex_e, NULL);
  pthread_mutex_init(&Mutex_E, NULL);
  pthread_mutex_init(&Mutex_w, NULL);
  pthread_mutex_init(&Mutex_W, NULL);
  pthread_mutex_init(&Loop, NULL);
  pthread_mutex_init(&Timer, NULL);
  pthread_mutex_init(&startThreads, NULL);

  e = createQueue();
  E = createQueue();
  w = createQueue();
  W = createQueue();

  // Part 1, taking file and making them to threads and starting the threads.
  FILE  *fpI;
  int arrSize = 0;
  fpI = fopen(argv[1], "r");
  char str[256];
  // First scan is just for how many threads and condvars need to be created.
  while(1) {
      fscanf(fpI, "%s", str);
      if(feof(fpI)){
        break;
      }
      fscanf(fpI, "%s", str);
      fscanf(fpI, "%s", str);
      arrSize++;
  }
  fclose(fpI);

  pthread_t pthread[arrSize];
  pthread_cond_t condThread[arrSize];
  int i = 0;
  // initalizes the condvars for each thread
  while (i < arrSize){
      pthread_cond_init(&condThread[i], NULL);
      i++;
  }
  // second scan stores infor into nodes and creates the threads
  FILE *fp;
  char buff[255];
  char dir;
  int load;
  int cross;
  fp = fopen(argv[1], "r");
  int numTrains = 0;

  while (1) {
     fscanf(fp, "%s", buff);
     if(feof(fp)){
       break;
     }
     dir = buff[0];
     fscanf(fp, "%s", buff);
     load = atoi(buff);
     fscanf(fp, "%s", buff);
     cross = atoi(buff);
     struct Node* node = createNode(numTrains, load, cross, dir, condThread[numTrains]);
     pthread_create(&pthread[numTrains], NULL, threadFunction, (void*)node);
     numTrains++;
   }
   fclose(fp);
   // Unlock all the threads
   pthread_cond_broadcast(&begin);
   // sleep before timer starts to make up
   // for lost time with many trains
   usleep(50000);
   if( clock_gettime( CLOCK_REALTIME, &start) == -1 ) {
     perror( "clock gettime" );
     exit( EXIT_FAILURE );
   }
   // dispatcher
   loop(numTrains, pthread);

   // destroys mutexs when trains are all finished crossing.
   pthread_mutex_destroy(&track);
   pthread_mutex_destroy(&Mutex_e);
   pthread_mutex_destroy(&Mutex_E);
   pthread_mutex_destroy(&Mutex_w);
   pthread_mutex_destroy(&Mutex_W);
   pthread_mutex_destroy(&Loop);
   pthread_mutex_destroy(&Timer);
   pthread_mutex_destroy(&startThreads);

   // Done!
   return 0;
}
