#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>

#define TCP_PORT 4950
#define BACKLOG 5
#define DATALEN 125
#define ERROR_RATE 75
#define BUFSIZE 60000
#define FALSE 0
#define TRUE 1

struct ack_so {
    uint8_t response;
    uint8_t packet_no;
};
