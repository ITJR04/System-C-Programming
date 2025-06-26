#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <assert.h>
#define MAX 100000
#define SIZE 10000

int n = SIZE;           
int v[SIZE][SIZE];      //2d array
pthread_mutex_t mutex;  //a mutex
int r[MAX];             //1d array to save results
int count = 0;          //number of items in the array r

void die(char *s)
{
    fprintf(stderr, "Error: %s\n", s);
    if (errno) perror("errno");
    exit(EXIT_FAILURE);
}

void check_pthread_return(int rv, char *msg)
{
    if (rv == 0) return;
    errno = rv;
    die(msg);
}
//TODO:
//implement the following function to append i*n + j to
//the end of array r
//and update count
//note multiple thread could call this function simutaneously

void append(int i, int j)
{
    pthread_mutex_lock(&mutex); // Lock mutex
    r[count++] = i * n + j;
    pthread_mutex_unlock(&mutex); // Unlock mutex
}

typedef struct thread_arg_tag {
    int row1, row2;
} thread_arg_t;

//TODO:
//implement the following function
//to test whether a 3x3 square centered at (i, j) is a lucky square

int lucky_square(int i, int j)
{
    if (i <= 0 || j <= 0 || i >= n - 1 || j >= n - 1) return 0; // 
    int nums[10] = {0};
    nums[v[i][j]]++; // location of the center of square
        int freq[10] = {0}; // checks frequency of the number ranging from 0 to 9 

        // checks and loops through the 3x3 square with center of (i,j)
        for (int k = 0; k < 9; k++) {
            int x = i - 1 + k / 3; 
            int y = j - 1 + k % 3;
            int val = v[x][y];

            if (val < 1 || val > 9 || freq[val] > 0) return 0; // returns that there are no perfect squares
            // if there happens to be duplicates or out of range values
            freq[val]++; // increment the specfied value in the 3x3 square
        }
            
    for (int k = 1; k <= 9; k++) // checks and makes sure all digits are shown once
    {
        if (nums[k] != 1) return 0;
    }

    return 1; // a lucky square is found
}

// print_square function is provided to help dubugging 
void print_square(int i, int j)
{
    printf("v[%d][%d] = %d\t", i, j, v[i][j]);
    for(int k = 0; k < 8; k++)
    {
        int val = v[i -1 + k /3][j - 1 + k %3];
        printf("%d ", val);
    }
    int k = 9;
    int val = v[i -1 + k /3][j - 1 + k %3];
    printf("%d\n", val);
}

//main for threads
void* thread_main(void* thread_arg) {
    thread_arg_t* arg = thread_arg;
    
    int row1 = arg->row1;
    int row2 = arg->row2;

    for(int i = row1; i<= row2; i++)
    for(int j = 1; j < n-1; j++)
    {
        if(lucky_square(i, j)) append(i, j);
    }
    pthread_exit(NULL);
}
//int_cmp used for qsort()
int int_cmp(const void *a, const void *b)
{
    return (*(int *)a - *(int *)b);
}

int main(int argc, char* argv[]) {

    if(argc!=3)
    {
        printf("Usage: %s seed n_threads\n", argv[0]);
        return 0;
    }

    int seed = atoi(argv[1]);
    int n_threads = atoi(argv[2]);
    srand(seed);

    //initialization
    for(int i = 0; i < n; i++)
    for(int j = 0; j < n; j++)
    {
        v[i][j] = rand() % 10;
    }
    //TODO: initialize something
    pthread_mutex_init(&mutex, NULL); // initialize the mutex

    pthread_t thread_arr[n_threads];
    thread_arg_t thread_args[n_threads];
    int rv;
    int rpt = n / n_threads; // gets the rows per threads

    // TODO: Set up thread arguments
    for(int i = 0; i < n_threads; i++) { // loop to distribute what threads are in charge of which rows
            thread_args[i].row1 = i * rpt; 
            if (i == n_threads - 1){ // if th last thread
                thread_args[i].row2 = n-1; // assign rows until the end of 2-D array
        }
        else{ // all other threads
            thread_args[i].row2 = (i + 1) * rpt - 1; // ending row of the thread is made sure to be before
            // the start of the next thread's starting row
        }
    };
    // TODO: Create the threads
    for(int i = 0; i < n_threads; i++) { // loop through creating n number of threads
            int rv = pthread_create(&thread_arr[i], NULL, thread_main, &thread_args[i]); // create a thread
            check_pthread_return(rv, "pthread_create"); // check if the thread was created successfully
        };

    // TODO: Join with the threads
    for(int i = 0; i < n_threads; i++) { // for all threads
            int rv = pthread_join(thread_arr[i], NULL); // wait for thread to finsh
            check_pthread_return(rv, "pthread_join"); // check if the thread finished successfully
        };

    // TODO: destroy something
    pthread_mutex_destroy(&mutex);

    //DO NOT CHANGE THE FOLLOWING CODE
    printf("Total lucky squares: %d\n", count);
    //sort r[] and print results
    qsort(r, count, sizeof(int), int_cmp);
    for(int i = 0; i< (10 < count? 10: count); i++)
    {
        printf("(%d, %d)\n", r[i] / n, r[i] % n);
    }

    return 0;
}