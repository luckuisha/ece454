
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "defs.h"
#include "hash.h"

#define SAMPLES_TO_COLLECT   10000000
#define RAND_NUM_UPPER_BOUND   100000
#define NUM_SEED_STREAMS            4

/* 
 * ECE454 Students: 
 * Please fill in the following team struct 
 */
team_t team = {
    "Team Name",                  /* Team name */

    "kyu bum kim",                    /* Member full name */
    "1003969100",                 /* Member student number */
    "kb.kim@mail.utoronto.ca",                 /* Member email address */
};

unsigned num_threads;
unsigned samples_to_skip;

class sample;

class sample {
  unsigned my_key;
 public:
  sample *next;
  unsigned count;

  sample(unsigned the_key){my_key = the_key; count = 0;};
  unsigned key(){return my_key;}
  void print(FILE *f){printf("%d %d\n",my_key,count);}
};

// This instantiates an empty hash table
// it is a C++ template, which means we define the types for
// the element and key value here: element is "class sample" and
// key value is "unsigned".  
hash<sample,unsigned> h;

typedef struct thread_args {
  int seed_start;
  int seed_end;
} thread_args;

void * smoke_hash(void *);

int  
main (int argc, char* argv[]){

  // Print out team information
  printf( "Team Name: %s\n", team.team );
  printf( "\n" );
  printf( "Student 1 Name: %s\n", team.name1 );
  printf( "Student 1 Student Number: %s\n", team.number1 );
  printf( "Student 1 Email: %s\n", team.email1 );
  printf( "\n" );

  // Parse program arguments
  if (argc != 3){
    printf("Usage: %s <num_threads> <samples_to_skip>\n", argv[0]);
    exit(1);  
  }
  sscanf(argv[1], " %d", &num_threads); // not used in this single-threaded version
  sscanf(argv[2], " %d", &samples_to_skip);

  // initialize a 16K-entry (2**14) hash of empty lists
  h.setup(14);

  // initialize pthread db
  pthread_t threads[num_threads];
  thread_args args[num_threads];

  int seeds_per_thread = NUM_SEED_STREAMS / num_threads;

  for (int i = 0; i < num_threads; i++){
    args[i].seed_start = i * seeds_per_thread;
    args[i].seed_end = args[i].seed_start + seeds_per_thread;
    pthread_create(&threads[i], NULL, smoke_hash, &args[i]);
  }

  for (int i = 0; i < num_threads; i++){
    pthread_join(threads[i], NULL);
  }

  // print a list of the frequency of all samples
  h.print();
}

void * smoke_hash(void * argument){

  int i,j,k;
  int rnum;
  unsigned key;
  sample *s;

  thread_args * arg = (thread_args *) argument;

  // process streams starting with different initial numbers
  for (i= arg->seed_start; i< arg->seed_end; i++){
    
    rnum = i;

    // collect a number of samples
    for (j=0; j<SAMPLES_TO_COLLECT; j++){

      // skip a number of samples
      for (k=0; k<samples_to_skip; k++){
        rnum = rand_r((unsigned int*)&rnum);
      }

      // force the sample to be within the range of 0..RAND_NUM_UPPER_BOUND-1
      key = rnum % RAND_NUM_UPPER_BOUND;

      // CRITICAL SECTIONE
      __transaction_atomic {

        // if this sample has not been counted before
        if (!(s = h.lookup(key))){
    
          // insert a new element for it into the hash table
          s = new sample(key);
          h.insert(s);
        }

        // increment the count for the sample
        s->count++;

      }
      // CRITICAL SECTION OVER
    }
  }

  return NULL;
}