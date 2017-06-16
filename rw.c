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

void initialize_readers_writer() {
    /*
     * Initialize the shared structures, including those used for
     * synchronization.
     */
}


void rw_read(char *value, int len) {
    printf("NOTHING IMPLEMENTED YET FOR rw_read\n");
}


void rw_write(char *value, int len) {
    printf("NOTHING IMPLEMENTED YET FOR rw_write\n");
}
