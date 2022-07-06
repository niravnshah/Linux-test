#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "myioctl.h"

int main()
{
    int fd;
    unsigned long value, number;

    printf("myioctl_test starting...\n");

    printf("Opening myioctl device...\n");
    fd = open("/dev/myioctl_device", O_RDWR);
    if(fd < 0) {
        printf("Cannot open myioctl_device.\n");
        return -1;
    }

    printf("myioctl_device opened successfully.\n");

    printf("Enter the Value to send\n");
    scanf("%ld",&number);
    printf("Writing Value to Driver\n");
    ioctl(fd, WR_VALUE, (int32_t*) &number);

    printf("Reading Value from Driver\n");
    ioctl(fd, RD_VALUE, (int32_t*) &value);
    printf("Value is %ld\n", value);

    close(fd);
    printf("Closed myioctl device...\n");
    return 0;
}