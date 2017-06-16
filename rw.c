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
    sem_init(&mutexRead, 0, 1);
    sem_init(&mutexWrite, 0, 1);
}

void rw_read(char *value, int len)
{
    // printf("NOTHING IMPLEMENTED YET FOR rw_read\n");
    // printf("Reader called\n");
    sem_wait(&mutexRead);
    if (data.num_reads == 1)
    {
        // printf("The read mutex is set\n");
        sem_wait(&mutexWrite);
    }
    sem_post(&mutexRead);

    read_resource(&data, value, len);

    sem_wait(&mutexRead);
    data.num_reads -= 1;
    // printf("The num_reads are %d\n",data.num_reads);
    if (data.num_reads == 0)
    {
        sem_post(&mutexWrite);
        // printf("the Db is released\n");
    }
    sem_post(&mutexRead);
    //non critical region
}

void rw_write(char *value, int len)
{
    // printf("Write is called\n");
    sem_wait(&mutexWrite);
    write_resource(&data, value, len);
    // printf("Written");
    sem_post(&mutexWrite);
}
