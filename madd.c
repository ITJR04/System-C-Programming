#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "matrix.h"

#define     NUM_THREADS     2

typedef struct {
    unsigned int id;
    TMatrix *m, *n, *t;
} thread_arg_t;

/* the main function of threads */
static void * thread_main(void * p_arg)
{
    int val, loop;
    // TODO
    thread_arg_t* info = (thread_arg_t*) p_arg;
    TMatrix *m = info->m;
    TMatrix *n = info->n;
    TMatrix *t = info->t;

    val = info->id;

    while(val < m->nrows){
        for(loop = 0; loop < n->ncols; loop++){
            t->data[val][loop] = n->data[val][loop] + m->data[val][loop];
        }
        val = val + NUM_THREADS;
    }


    return NULL;
}

/* Return the sum of two matrices. The result is in a newly creaed matrix. 
 *
 * If a pthread function fails, report error and exit. 
 * Return NULL if something else is wrong.
 *
 * Similar to addMatrix, but this function uses 2 threads.
 */
TMatrix * addMatrix_thread(TMatrix *m, TMatrix *n)
{
    if (    m == NULL || n == NULL
         || m->nrows != n->nrows || m->ncols != n->ncols )
        return NULL;

    TMatrix * t = newMatrix(m->nrows, m->ncols);
    if (t == NULL)
        return t;

    // TODO
    int val, loop;
    pthread_t threads[NUM_THREADS];
    thread_arg_t lst[NUM_THREADS];

    for(loop = 0; loop < NUM_THREADS; loop++){
        lst[loop].id = loop;
        lst[loop].m = m;
        lst[loop].n = n;
        lst[loop].t = t;

        val = pthread_create(&threads[loop], NULL, thread_main, &lst[loop]);
        if(val){

            printf("Error");
            exit(-1);

        }
    }
    for(loop = 0; loop < NUM_THREADS; loop++){
        val = pthread_join(threads[loop], NULL);
        if(val){
            printf("ERROR");
            exit(-1);
        }
    }



    return t;
}
