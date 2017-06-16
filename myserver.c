#include <assert.h> 
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "network.h"
#include "requests.h"
#include "server.h"
#include "meetup.h"


/*
 * This data needed to manage the pool of Pthreads available to
 * respond to client requests. It is a little tricky to determine
 * when a thread is completed, but we take advantage here that
 * server threads (other than the "heartbeat" thread) are only ever
 * created when some file descriptor is opened for a client request.
 */
int thread_fd[MAX_THREADS];
pthread_t thread_pool[MAX_THREADS];
pthread_mutex_t pool_mutex;
int meet_size = 1;
int meet_listen = 1;

void *heartbeat(void *);
void *server_action(void *);


/*
 * Some functions to help with finding the first available thread from the
 * "pool" of threads (i.e., really the array named "thread_pool").
 */


int grab_thread_index() {
    int i;
    int result = -1;
    
    pthread_mutex_lock(&pool_mutex);
    for (i = 0; i < MAX_THREADS; i++) {
        if (thread_fd[i] == -1) {
            result = i;
            break;
        }
    }
    pthread_mutex_unlock(&pool_mutex);

    return result;
}


void set_thread_fd(int index, int fd) {
    if (0 <= index && index < MAX_THREADS) {
        thread_fd[index] = fd;
    }
}


void return_thread_index(int fd) {
    int i;

    pthread_mutex_lock(&pool_mutex);
    for (i = 0; i < MAX_THREADS; i++) {
        if (thread_fd[i] == fd) {
            thread_fd[i] = -1;
            break;
        }
    }
    pthread_mutex_unlock(&pool_mutex);
}


/*
 * TO DO:
 *
 * For each of the cases in the switch statement, you'll need to
 * call appropriate bounded-buffer functions and work with
 * the results to compute what is needed.
 *
 * YOU ARE NOT TO ADD ANY POSIX SYNCHRONIZATION OPERATIONS TO THIS
 * FUNCTION. All synchronization MUST take place in bbuffer.c.
 */
void *server_action(void *arg) {
    int comm_fd = *(int *)arg;
    char str[OUTPUT_BUFFER_SIZE];
    char operand[OUTPUT_BUFFER_SIZE];
    char result_message[OUTPUT_BUFFER_SIZE];
    char temp_result[OUTPUT_BUFFER_SIZE];
    char *separator = "";
    int  value;
    int  i, sum;
    pc_op_t operation;

    bzero(str, OUTPUT_BUFFER_SIZE);
    read(comm_fd, str, OUTPUT_BUFFER_SIZE);
    operation = parse_request(str, operand);
    sprintf(result_message, "%s\n", "GET OFF MY LAWN!");
    switch(operation) {
        case READ: 
            printf("server: READ\n"); 
            rw_read(operand, OUTPUT_BUFFER_SIZE);
            sprintf(result_message, "server: READ -- value is '%s'\n", operand);
            break;
        case WRITE: 
            printf("server: WRITE\n"); 
            rw_write(operand, OUTPUT_BUFFER_SIZE);
            sprintf(result_message, 
                "server: WRITE -- value written is '%s'\n", operand);
            break;
        case MEETUP: 
            printf("server: MEETUP\n"); 
            printf("server: parameter is %s\n", operand);
            join_meetup(operand, OUTPUT_BUFFER_SIZE);
            sprintf(result_message, 
                "server: MEETUP -- value shared is '%s'\n", operand);
            break;
        case DEBUG: 
            printf("server: DEBUG\n"); 
            /* TO DO something here for DEBUG */
            break;
        case UNKNOWN:
        default:
            printf("server: UNKNOWN\n"); 
            break;
    }

    send_http_response(comm_fd, result_message);
    close(comm_fd);
    return_thread_index(comm_fd);
}



void initialize() {
    int status;
    int i;

    /*
     * Each thread created by server-loop in main() will have the file
     * description # for which it was generated stored in thread_fd.
     * This array will also server as a proxy for what threads are available,
     * i.e., if thread_fd[i] == -1 then thread[i] is available.
     * 
     */
    for (i = 0; i < MAX_THREADS; i++) {
        thread_fd[i] = -1;
    }

    status = pthread_mutex_init(&pool_mutex, NULL);
    if (status != 0) {
        fprintf(stderr, "Error creating pool_mutex\n");
        exit(1);
    }
   
    /*
     * This function calls YOUR code for initializing needed readers-writer
     * data and meetup/barrier data plus synchronization structures.
     */
    initialize_readers_writer();
    initialize_meetup(meet_size, meet_listen);
}


void *heartbeat(void *arg) {
    int interval = *(int *)arg;

    for (;;) {
        printf("<3\n");
        sleep(interval);
    }
}


int main(int argc, char *argv[]) {

    /* Vars for setting up heartbeat */
    pthread_t heart;
    int interval = 5;

    /* Vars for setting up server threads */
    char str[OUTPUT_BUFFER_SIZE];
    int listen_fd, comm_fd;
    int i;
    int thread_index;
    int num_threads = 0;
    int bind_result;
    void *result;
    int status;
    

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--meetup") == 0 & (i + 1) < argc) {
           meet_size = atoi(argv[i+1]);
        } else if (strcmp(argv[i], "--meetfirst") == 0) {
            meet_listen = MEET_FIRST;
        } else if (strcmp(argv[i], "--meetlast") == 0) {
            meet_listen = MEET_LAST;
        }
    }

    initialize();
    

    /*
     * First start the heartbeat -- just to convince ourselves that
     * "accept()" is not blocking the whole process... 
     *
     */

    if (pthread_create(&heart, 0, heartbeat, (void *)&interval) < 0) {
        fprintf(stderr, "Could not create heartbeat thread\n");
        return 1; 
    }


    /*
     * Now set up server threads as needed -- i.e., as separate clients
     * connect to the server.
     */

    listen_fd = setup_listener();

    while (comm_fd = accept(listen_fd, (struct sockaddr*) NULL, NULL)) {

        thread_index = grab_thread_index();
        if (thread_index == -1) {
            fprintf(stderr, "No more threads available.\n");
            exit(1);
        }

        set_thread_fd(thread_index, comm_fd);

        if (pthread_create(&thread_pool[thread_index], 0, 
                server_action, &comm_fd) < 0)
        {
            fprintf(stderr, "Could not create thread %d\n", num_threads);
            exit(1);
        }

        printf("Created server thread %d\n", thread_index);
    }
}
