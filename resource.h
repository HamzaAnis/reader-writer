#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#define DELAY_READ  2  /* two seconds */
#define DELAY_WRITE 4  /* five seconds */
#define MAX_RESOURCE_SIZE 1000
#define MAX_LABEL_LEN 80

typedef struct resource {
    char value[MAX_RESOURCE_SIZE];
    char label[MAX_LABEL_LEN];
    int  num_reads;    
    int  num_writes;
} resource_t;

void init_resource(resource_t *, char *);
void read_resource(resource_t *, char*, int);
void write_resource(resource_t *, char *, int);
void print_stats(resource_t *);

#endif
