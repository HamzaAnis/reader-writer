#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "resource.h"

void init_resource(resource_t *r, char *l) {
    assert( r != NULL );

    memset(r->value, 0, MAX_RESOURCE_SIZE);
    memset(r->label, 0, MAX_LABEL_LEN);
    strncpy(r->label, l, MAX_LABEL_LEN-1);

    r->num_writes = 0;
    r->num_reads = 0;
}


void read_resource(resource_t *r, char *v, int len) {
    assert( r != NULL );

    memcpy(v, r->value, 
        (MAX_RESOURCE_SIZE < len ? MAX_RESOURCE_SIZE : len));
    sleep(DELAY_READ);
    r->num_reads++;
}


void write_resource(resource_t *r, char *v, int len) {
    assert( r != NULL );

    memcpy(r->value, v,
        (MAX_RESOURCE_SIZE < len ? MAX_RESOURCE_SIZE : len));
    sleep(DELAY_WRITE);
    r->num_writes++;
}


void print_stats(resource_t *r) {
    assert( r != NULL );

    printf("Resource: %s; # reads: %d; # writes: %d\n",
        r->label, r->num_reads, r->num_writes);
}
