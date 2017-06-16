/*Required Headers*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include "rw.h"
#include "resource.h"

/*
 * Declarations for reader-writer shared variables -- plus concurrency-control
 * variables -- must START here.
 */

static resource_t data;
sem_t mutexRead;
sem_t mutexWrite;

void initialize_readers_writer()
{
    /*
     * Initialize the shared structures, including those used for
     * synchronization.
     */
    init_resource(&data, "Reader Writer");
    //  print_stats(&data);
    //initialzing the mutex for reading
    sem_init(&mutexRead, 0, 1);
    //initialzing the mutex for writing
    sem_init(&mutexWrite, 0, 1);
}

void rw_read(char *value, int len)
{
    // printf("NOTHING IMPLEMENTED YET FOR rw_read\n");
    // printf("Reader called\n");
    //taking the lock
    sem_wait(&mutexRead);
    //If only one is reading
    if (data.num_reads == 1)
    {
        // printf("The read mutex is set\n");
        //take the lock for the writing
        sem_wait(&mutexWrite);
    }
    //realease the lock
    sem_post(&mutexRead);

    //read the resource
    read_resource(&data, value, len);

    //take the lock
    sem_wait(&mutexRead);
    data.num_reads -= 1;
    // printf("The num_reads are %d\n",data.num_reads);
    if (data.num_reads == 0)
    {
        //releasing the write lock
        sem_post(&mutexWrite);
        // printf("the Db is released\n");
    }
    //releasing the read lock
    sem_post(&mutexRead);
    //non critical region
}

void rw_write(char *value, int len)
{
    // printf("Write is called\n");
    //taking lock for write
    sem_wait(&mutexWrite);
    write_resource(&data, value, len);
    // printf("Written");
    //Releasing the lock for write
    sem_post(&mutexWrite);
}
