#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#define NUM_CONSUMERS 25

#define FLAG 0x3FFFFFE	//the flag to check whether all 25 consumers have finished CHECK_BIT

#define SET_BIT(v, i)   (v) |= (1 << (i))	//used by consumer i to set the bit on v
#define CHECK_BIT(v, i)   (v) & (1 << (i))	//used by consumer i to check the bit on v

typedef struct {
    int     data;				//integer to be tested
    int     ready;				//data ready for testing?
    int     checked;			//used by SET_BIT and CHECK_BIT 
    pthread_mutex_t mutex;		//mutex
    pthread_cond_t ready_cond;	//ready condition varible
    pthread_cond_t processed_cond;	//processed condition variable
} data_t;

typedef struct {
    int     id;					//id of the thread
    int     p;					//prime number for the thread
    data_t  *pdata;				//pointer to the data of the type data_t
} thread_arg_t;

//consumer thread tests if the data can be divided by the prime number owned by the thread
void * consumer(void *t) 
{
    thread_arg_t * arg = t;

    // get parameters
    int my_id = arg->id;
    int p = arg->p;
    data_t *pdata = arg->pdata;

    int done = 0;

    while (! done) 
	{
		//Fill in code below on mutex and condition variable

        // lines beween lock and unlock try to get data
        pthread_mutex_lock(&pdata->mutex);

        // wait if data is not ready, or it is ready but I already checked
        while (! pdata->ready || CHECK_BIT(pdata->checked, my_id)) {
            pthread_cond_wait(&pdata->ready_cond, &pdata->mutex);
        }

		//if the data produced is negative, we set done to 1
        if (pdata->data < 0) {
            done = 1;
        }
        else 
		{ 
			if ((pdata->data % p) == 0) 
			{
				//fill in the code below
                printf("%d is not a prime.\n", pdata->data);
                pdata->ready = 0;
                pthread_cond_signal(&pdata->processed_cond);
            }
            else 
			{
				//fill in the code below
                SET_BIT(pdata->checked, my_id);
                if(pdata->checked == FLAG)
                {
                    printf("%d is a prime.\n", pdata->data);
                    pdata->ready = 0;
                    pthread_cond_signal(&pdata->processed_cond);
                }
            }
        }
        pthread_mutex_unlock(&pdata->mutex);
    }
    return NULL;
}

//read an integer. if it is not in the range it will be set to -1
int read_integer()
{
        printf("Enter an integer: (100 to 10000)\n");
        int v;
        
        if (scanf("%d", &v) != 1) {
            v = -1;
        }   
        
        //make sure v is in the range
        if(v < 100 || v >10000) v = -1;

	return v;
}

void *producer(void *t) 
{
    thread_arg_t * arg = t;

    // get parameters
    data_t *pdata = arg->pdata;

    int done = 0;
    while (! done) 
	{
		//Fill in code below on mutex and condition variable
        pthread_mutex_lock(&pdata->mutex);
        while (pdata->ready) {
            pthread_cond_wait(&pdata->processed_cond, &pdata->mutex);
        }

		//read an integer to v
		int v = read_integer();
		//fill in the code below to produce data and wake up consumers
        pdata->data = v;
        pdata->ready = 1;
        pdata->checked = 0;
        pthread_cond_broadcast(&pdata->ready_cond);
        pthread_mutex_unlock(&pdata->mutex);
        done = (v < 0);
    }

    return NULL;
}

//Do not change the main() function
int main (int argc, char *argv[])
{

    //There are 25 pirmes below 100
    static int primes[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97};
    int i, rv;

    data_t  data;

    pthread_t threads[NUM_CONSUMERS + 1];
    thread_arg_t thread_args[NUM_CONSUMERS + 1];

    /* Initialize mutex and condition variable objects */
    pthread_mutex_init(&data.mutex, NULL);
    pthread_cond_init(&data.ready_cond, NULL);
    pthread_cond_init(&data.processed_cond, NULL);

    data.ready = 0; // no data is ready yet

    // create a producer
    thread_args[0].id = 0;
    thread_args[0].p = 0;
    thread_args[0].pdata = &data;
    rv = pthread_create(&threads[0], NULL, producer, &thread_args[0]);
    assert(rv == 0);

    // create consumers
    for (i = 1; i <= NUM_CONSUMERS; i ++) {
        // prepare arguments
        thread_args[i].id = i;
        thread_args[i].p = primes[i-1]; 
        thread_args[i].pdata = &data;

        rv = pthread_create(&threads[i], NULL, consumer, &thread_args[i]);
        assert(rv == 0);
    }

    /* Wait for all threads to complete */
    for (i = 0; i <= NUM_CONSUMERS; i++) {
	    pthread_join(threads[i], NULL);
    }

    /* Clean up and exit */
    pthread_mutex_destroy(&data.mutex);
    pthread_cond_destroy(&data.ready_cond);
    pthread_cond_destroy(&data.processed_cond);
    return 0;
} 
