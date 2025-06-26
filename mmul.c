#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "matrix.h"

// Search TODO to find the locations where code needs to be completed

#define     NUM_THREADS     2

typedef struct {
    unsigned int id;
    TMatrix *m, *n, *t;
} thread_arg_t;

static void * thread_main(void * p_arg)
{


    // TODO
    // create a pointer to the struct.


    thread_arg_t *my_data = p_arg;
    int val;


    val = my_data->id;


    TMatrix *m = my_data->m;
    TMatrix *n = my_data->n;
    TMatrix *t = my_data->t;


    while(val < m->nrows){

        for(int i = 0; i < n->ncols; i++){

            TElement sum = (TElement) 0;

            for(int j = 0; j < m->ncols; j++){


                sum += m->data[val][j] * n->data[j][i];

            }

            t->data[val][i] = sum;
        }


        val += NUM_THREADS;
    } 

    return NULL;
}

/* Return the sum of two matrices.
 *
 * If any pthread function fails, report error and exit. 
 * Return NULL if anything else is wrong.
 *
 * Similar to mulMatrix, but with multi-threading.
 */

TMatrix * mulMatrix_thread(TMatrix *m, TMatrix *n)
{
    if (    m == NULL || n == NULL
         || m->ncols != n->nrows )
        return NULL;

    TMatrix * t = newMatrix(m->nrows, n->ncols);
    if (t == NULL)
        return t;

    // TODO


    int check;

    pthread_t threads[NUM_THREADS];

    thread_arg_t thread_args[NUM_THREADS];


	for(int i = 0; i < NUM_THREADS; i++ ){


        thread_args[i].id = i;

        thread_args[i].m = m;

        thread_args[i].n = n;

        thread_args[i].t = t;
      
        check = pthread_create(&threads[i], NULL, thread_main, &thread_args[i]);


        if (check){

            printf("ERROR");
            exit(-1);
        }
    }

    for(int i = 0; i < NUM_THREADS; i++){

        pthread_join(threads[i], NULL);
    }
    
    return t;
}