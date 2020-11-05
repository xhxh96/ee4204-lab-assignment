// headfile for TCP program
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

#define NEWFILE (O_WRONLY|O_CREAT|O_TRUNC)
#define TCP_PORT 4950
#define BACKLOG 5
#define DATALEN 500
#define ERROR_RATE 0
#define BUFSIZE 60000
#define PACKLEN 508
#define HEADLEN 8

//data packet structure
struct pack_so {
    uint32_t num;	// the sequence number
    uint32_t len;	// the packet length
    char data[DATALEN];	//the packet data
};

struct ack_so {
    uint8_t response;
    uint8_t packet_no;
};
