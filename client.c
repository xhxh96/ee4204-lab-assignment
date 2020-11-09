#include "headsock.h"

float str_cli(FILE *fp, int network_socket, long *len);
void tv_sub(struct timeval *out, struct timeval *in);

int main(int argc, char **argv) {
    int max_iteration = 1;

    // experiment mode
    if (argc == 2) {
        max_iteration = 60;
    }

    for (int i = 0; i < max_iteration; i++) {
        sleep(1);

        FILE *fp;
        FILE *output;
        float ti, rt;
        long len;
        
        // create socket
        int network_socket = socket(AF_INET, SOCK_STREAM, 0);

        // specify address for socket
        struct sockaddr_in server_address;
        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(TCP_PORT);
        server_address.sin_addr.s_addr = INADDR_ANY;
        
        int connection_status = connect(network_socket, (struct sockaddr *)&server_address, sizeof(server_address));

        if (connection_status == -1) {
            printf("There's an error making a connection to the remote socket\n");
            close(network_socket);
            exit(1);
        } else {
            printf("Connected to port %d...\n", TCP_PORT);
        }

        if ((fp = fopen("myfile.txt", "r+t")) == NULL) {
            printf("File does not exist!\n");
            exit(0);
        }

        ti = str_cli(fp, network_socket, &len);
        rt = (len / (float)ti);

        printf("File successfully transferred!\nTime Elapsed: %.3f ms\tData Sent: %d bytes\tData Rate: %f KB/s\n", ti, (int)len, rt);

        close(network_socket);
        fclose(fp);

        // Save info to csv: Time Elapsed, Data Rate, Data Unit, Error Rate
        output = fopen("output.csv", "a+");
        fprintf(fp, "%.3f, %f, %d, %d\n", ti, rt, DATALEN, ERROR_RATE);
        fclose(output);
    }

    exit(0);
}

float str_cli(FILE *fp, int network_socket, long *len) {
    long current_byte = 0;
    char packet[DATALEN];
    struct ack_so ack;
    int send_status, ack_status, packet_length;
    int packet_count = 1;
    float time_inv = 0.0;
    struct timeval sendt, recvt;

    // process file
    fseek(fp, 0, SEEK_END); // read file to eof and append 2 bytes to accomodate EOF escape char
    long fileSize = ftell(fp);   // set size of file
    rewind(fp); // reset stream from eof to beginning of file
    printf("The file length is %d bytes.\n", (int)fileSize);
    printf("The packet length is %d bytes. \n", DATALEN);
    printf("File will be sent with over in %d packets.\n", (int)(ceil((double)fileSize / DATALEN)));

    // allocate memory to fit entire file
    char *buf = (char *)malloc(fileSize);
    if (buf == NULL) {
        printf("Error allocating memory!\n");
        exit(2);
    }

    // copy file to memory
    fread(buf, 1, fileSize, fp);
    buf[fileSize] = '\0';   // append eof character
    gettimeofday(&sendt, NULL);

    // starts sending packets
    while (current_byte <= fileSize) {
        if (fileSize + 1 - current_byte <= DATALEN) {
            packet_length = fileSize + 1 - current_byte;
        } else {
            packet_length = DATALEN;
        }
        
        // transmit and retransmit if ACK received is negative
        do {
            printf("Sending packet #%d...\n", packet_count);
            memcpy(packet, buf + current_byte, packet_length);    // copy file from memory buffer to packet
            send_status = send(network_socket, &packet, packet_length, 0);  // send packet

            if (send_status == -1) {
                printf("Unable to send packet #%d!\n", packet_count);
                exit(1);
            }

            ack_status = recv(network_socket, &ack, 2, 0);
            if (ack_status == -1) {
                printf("Unable to receive acknolwedgement for packet #%d!\n", packet_count);
            }
            
            if (ack.response == 1) {
                printf("Server Error: Packet #%d is damaged!\n", ack.packet_no);
            }
        } while (ack_status == -1 || ack.response == 1);

        current_byte += packet_length;
        packet_count++;
    }
    gettimeofday(&recvt, NULL);
    *len = current_byte;
    tv_sub(&recvt, &sendt);
    time_inv += (recvt.tv_sec) * 1000.0 + (recvt.tv_usec) / 1000.0;
    return time_inv;
}

void tv_sub(struct timeval *out, struct timeval *in) {
    if ((out->tv_usec -= in->tv_usec) < 0) {
        --out->tv_usec;
        out->tv_usec += 1000000;
    }
    out->tv_sec -= in->tv_sec;
}