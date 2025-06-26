#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>

#define MAX 80
#define SET_BIT(v, i) (v) |= (1 << (i))
#define CHECK_BIT(v, i) (v) & (1 << (i))

typedef struct {
  char data;                     // player's guess (a letter)
  char *display;                 // current game status display
  int ready;                     // new letter ready
  int checked;                   // used by SET_BIT and CHECK_BIT
  int flag;                      // flag to indicate all checked
  pthread_mutex_t mutex;         // mutex
  pthread_cond_t ready_cond;     // condition variable that assitants wait on
  pthread_cond_t processed_cond; // condition variable that the player waits on
} data_t;

typedef struct {
  int id;        // id
  char letter;   // letter assigned
  data_t *pdata; // pointer to data_t
} thread_arg_t;

void *assistant(void *t) {
  thread_arg_t *arg = t;
  int my_id = arg->id;
  int letter = arg->letter;
  data_t *pdata = arg->pdata;
  int done = 0;

  do {
    // TODO: fill in code below
    pthread_mutex_lock(&pdata->mutex); // assistant aquires lock
        while (!pdata->ready) { // wait for players guess
      pthread_cond_wait(&pdata->ready_cond, &pdata->mutex); // untill the player guesses threads must wait
    }
    if (pdata->data < 0) {
      done = 1; // game is finished
    } else {
      // TODO: fill in code below
    if (pdata->data == letter) { // if the players quessed word has a letter in the right spot
        pdata->display[my_id] = letter; // Update the display array
        pdata->checked |= (1 << my_id); // signify that this position in the word has been checked
      }
    }
    // TODO: fill in code below
     if (pdata->checked == pdata->flag) { //checks if all assistantss have finished evaluating the player guess
      pdata->ready = 0; // Reset ready for the next guess
      pthread_cond_signal(&pdata->processed_cond); // Notify player that the processing of the guess is done
     }
     pthread_mutex_unlock(&pdata->mutex); 
  } while (!done);
  pthread_exit(NULL);
}

void *player(void *t) {
  thread_arg_t *arg = t;
  data_t *pdata = arg->pdata;
  int n = strlen(pdata->display);

  int done = 0;
  char v = 'a';

  do {
    // TODO: fill in code below
    pthread_mutex_lock(&pdata->mutex);

    pdata->data = v; // sets players guess 
    pdata->ready = 1; // signifies assistants are ready
    pdata->checked = 0; // sets the checked number of letters to zero

    pthread_cond_broadcast(&pdata->ready_cond); //signals to all assistants that the player guess is ready

    while (pdata->ready) { // wait for assistants to process players guess
      pthread_cond_wait(&pdata->processed_cond, &pdata->mutex);
    }

    printf("%s\n", pdata->display);
    int count = 0;
    for (int i = 0; i < n; i++) {
      if (pdata->display[i] != '-')
        count++;
    }
    if (count == n)
      v = -1;
    // TODO: fill in code below
    pthread_mutex_unlock(&pdata->mutex);
    if (v > 0)
      printf("%c\n", v);
    done = (v < 0);
    v++;
  } while (!done);
  pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
  int i, rv;
  data_t data;

  assert(argc == 2);
  int n = strlen(argv[1]);
  assert(n < MAX);
  char *word = argv[1];
  for (i = 0; i < n; i++)
    assert(word[i] >= 'a' && word[i] <= 'z');
  char display[MAX] = {0};

  int flag = 0;
  for (i = 0; i < n; i++)
    SET_BIT(flag, i + 1);
  for (i = 0; i < n; i++)
    display[i] = '-';

  data.ready = 0; // no data is ready yet
  data.display = display;
  data.flag = flag;

  pthread_t threads[n + 1];
  thread_arg_t thread_args[n + 1];

  // TODO: fill in code below
  // Initialize mutex and condition variable objects
  pthread_mutex_init(&data.mutex, NULL); // initializes mutex
  pthread_cond_init(&data.ready_cond, NULL); // initializes the ready for guess condition
  pthread_cond_init(&data.processed_cond, NULL); // initializes processed guess condition

  // TODO: fill in code below
  // create assistant threads
  for (i = 0; i < n; i++) { // loop through the creation of n number of assistants
    thread_args[i].id = i; // the goven thread id is the unique i in range of the word to guess
    thread_args[i].letter = word[i]; // the given argument for the specific letter for the ith assistant to watch for
    thread_args[i].pdata = &data; // thread is given access to the player data (guess)
    rv = pthread_create(&threads[i], NULL, assistant, &thread_args[i]); // creates the assistant
    assert(rv == 0); // assserts no error in creation of the assistant thread
  }
  // create the player thread
  thread_args[n].id = n;
  thread_args[n].letter = 0;
  thread_args[n].pdata = &data;
  rv = pthread_create(&threads[n], NULL, player, &thread_args[n]);
  assert(rv == 0);

  // TODO: fill in code below
  // Wait for all threads to complete
  for (i = 0; i <= n; i++) { // loops through the threads
    pthread_join(threads[i], NULL); // wait for thread i to finish
  }
  // TODO: fill in code below
  // Clean up and exit
  pthread_mutex_destroy(&data.mutex); // destroys created mutex
  pthread_cond_destroy(&data.ready_cond); // destroys ready for guess conditon
  pthread_cond_destroy(&data.processed_cond); // destroys proccessed guess condition

  return 0;
}