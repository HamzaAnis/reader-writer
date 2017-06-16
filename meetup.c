/*Required Headers*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include "meetup.h"
#include "resource.h"

/*
 * Declarations for barrier shared variables -- plus concurrency-control
 * variables -- must START here.
 */

static resource_t codeWordResource;

int hipCount = 0;                                // initialized when initiize meetup called
int meetFirst = 0;                               //check whether meetupfirst or last
int numOfHipstersArrived = -1;                   //number of hipsters on the server
pthread_mutex_t threadHipsterMutex[MAX_THREADS]; //mutex locks
pthread_mutex_t lock;                            //mutex lock
pthread_t threadHipster[MAX_THREADS];            //Thread for differenet hipsters
char buffer[40];                                 //reading the va;ue

char *CodeValue; //CodeWord
int length;      //lenght of the value

void *Barrier(void *arg) //here the codeword is decide andsent to others
{
    int threadNumber = *(int *)arg;
    if (threadNumber == 0 && meetFirst == 1) //when 1st hipster and meetfirst
    {
        pthread_mutex_lock(&lock); //taking lock
        // printf("\t\t%s\n", CodeValue);
        write_resource(&codeWordResource, CodeValue, length); //writing into the resource
        pthread_mutex_unlock(&lock);                          //releasing lock
    }
    if (threadNumber == hipCount && meetFirst == 0) //when meetlast and last thread then copy to thread
    {
        // printf("\t\t%s\n", CodeValue);
        write_resource(&codeWordResource, CodeValue, length); //writing to reasource
    }
    // printf("Thread number is %d and hipsCount is %d\n", threadNumber, hipCount);
    pthread_mutex_unlock(&threadHipsterMutex[threadNumber]); //releasing locks
    while (numOfHipstersArrived != hipCount)                 //This is the barrier and all threads will not proceed unless this is true
    {
    }
    read_resource(&codeWordResource, buffer, 40); //reading fromt resource
    printf("The code word is %s\n", codeWordResource.value);
    // print_stats(codeWordResource);
}

void initialize_meetup(int n, int mf)
{
    char label[100];
    int i;

    if (n < 1)
    {
        fprintf(stderr, "Who are you kidding?\n");
        fprintf(stderr, "A meetup size of %d??\n", n);
        exit(1);
    }

    /*
     * Initialize the shared structures, including those used for
     * synchronization.
     */
    //  initializng values
    hipCount = n;
    meetFirst = mf;

    for (i = 0; i < hipCount; i++)
    {

        int status = pthread_mutex_init(&threadHipsterMutex[i], NULL);
        if (status != 0)
        {
            fprintf(stderr, "Error creating Hipster Mutex\n");
            exit(1);
        }
    }
    pthread_mutex_init(&lock, NULL);
    //Locking all the threads
    for (i = 0; i < hipCount; i++)
    {
        pthread_mutex_lock(&threadHipsterMutex[i]);
    }
}

void join_meetup(char *value, int len)
{
    // printf("NOTHING IMPLEMENTED YET FOR join_meetup\n");
    //creating thread for hipster
    if (pthread_create(&threadHipster[numOfHipstersArrived], 0, Barrier, (void *)&numOfHipstersArrived) < 0)
    {
        fprintf(stderr, "Could not create %d thread\n", numOfHipstersArrived);
        return;
    }
    //When a new hipster saving it's value and len to use later
    CodeValue = value;
    length = len;
    // printf("In main thread is %d\n", numOfHipstersArrived);
    numOfHipstersArrived++;
}
