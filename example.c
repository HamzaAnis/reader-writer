#include <string.h>
#include <stdio.h>
#include "resource.h"

#define BUFFER_SIZE 40

int main(int argc, char *argv[]) {
    char buffer[BUFFER_SIZE];
    resource_t r;

    init_resource(&r, "*** example resource ***");
    print_stats(&r);    

    strncpy(buffer, "csc 360 A#2", BUFFER_SIZE);
    write_resource(&r, buffer, BUFFER_SIZE);
    print_stats(&r);    

    strncpy(buffer, "<nothing>", BUFFER_SIZE);
    printf("contents of buffer before call to read_resource: %s\n",
        buffer);
    read_resource(&r, buffer, BUFFER_SIZE);
    printf("contents of buffer before call to read_resource: %s\n",
        buffer);
            read_resource(&r, buffer, BUFFER_SIZE);
    print_stats(&r);    
}
