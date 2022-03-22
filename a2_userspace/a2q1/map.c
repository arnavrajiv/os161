/* map.c
 * ----------------------------------------------------------
 *  CS350
 *  Assignment 1
 *  Question 1
 *
 *  Purpose:  Gain experience with threads and basic
 *  synchronization.
 *
 *  YOU MAY ADD WHATEVER YOU LIKE TO THIS FILE.
 *  YOU CANNOT CHANGE THE SIGNATURE OF MultithreadedWordCount.
 * ----------------------------------------------------------
 */
#include "data.h"

#include <stdlib.h>
#include <string.h>
#include <stdatomic.h>

// start code here

_Atomic int wordCount = 0;
//static unsigned _Atomic wordCount = ATOMIC_VAR_INIT(0);
//volatile int count = 1;
// end code

/* --------------------------------------------------------------------
 * MultithreadedWordCount
 * --------------------------------------------------------------------
 * Takes a Library of articles containing words and a word.
 * Returns the total number of times that the word appears in the
 * Library.
 *
 * For example, "There the thesis sits on the theatre bench.", contains
 * 2 occurences of the word "the".
 * --------------------------------------------------------------------
 */

char * word;

struct threadData {
  struct Library *library;
  unsigned int idx;

};

void *getWordCount(void *param) {
  struct threadData *thread_t = (struct threadData *)param;
  unsigned int idx = thread_t->idx;
  struct Library *library = thread_t->library;
  unsigned int index = idx * (library->numArticles) / NUMTHREADS;
  unsigned int index_bound = (idx + 1) * (library->numArticles) / NUMTHREADS;

  while(index < index_bound) {
	  struct Article * article = library->articles[index];

	  for ( unsigned int j = 0; j < article->numWords; j++) {

      if ( !strcmp( article->words[j], word) ) {
        atomic_fetch_add_explicit(&wordCount, 1, memory_order_relaxed);

	    }
	  }
    index++;
  }

  pthread_exit(NULL);
}

size_t MultithreadedWordCount( struct  Library * library, char * wrd)
{
    /* XXX FILLMEIN
     * Also feel free to remove the printf statement
     * to improve time */
  pthread_t threads[NUMTHREADS];
  struct threadData thread_t[NUMTHREADS];
  int i = 0;
  word = wrd;

  while(i < NUMTHREADS) {
    thread_t[i].idx = i;
    thread_t[i].library = library;
    pthread_create(&threads[i], NULL, getWordCount, (void *)&thread_t[i]);
    i++;
  }

  i = 0;
  do {
    pthread_join(threads[i], NULL);
    i++;
  } while(i < NUMTHREADS);

  return wordCount;
}
