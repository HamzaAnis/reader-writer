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
    sem_init(&mutexRead, 0, 1); //initialzing the mutex for reading
    sem_init(&mutexWrite, 0, 1); //initialzing the mutex for writing
}

void rw_read(char *value, int len)
{
    // printf("NOTHING IMPLEMENTED YET FOR rw_read\n");
    // printf("Reader called\n");
    sem_wait(&mutexRead); //taking the lock
    //If only one is reading
    if (data.num_reads == 1)
    {
        // printf("The read mutex is set\n");
        sem_wait(&mutexWrite); //take the lock for the writing
    }
    sem_post(&mutexRead); //realease the lock

    read_resource(&data, value, len); //read the resource

    sem_wait(&mutexRead); //take the lock
    data.num_reads -= 1;
    // printf("The num_reads are %d\n",data.num_reads);
    if (data.num_reads == 0)
    {
        sem_post(&mutexWrite); //releasing the write lock
        // printf("the Db is released\n");
    }
    sem_post(&mutexRead); //releasing the read lock
    //non critical region
}

void rw_write(char *value, int len)
{
    // printf("Write is called\n");
    sem_wait(&mutexWrite); //taking lock for write
    write_resource(&data, value, len);
    // printf("Written");
    sem_post(&mutexWrite); //Releasing the lock for write
}
