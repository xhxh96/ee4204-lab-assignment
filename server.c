#include "headsock.h"

void str_ser(int sockfd);
int ack_num(int error_rate);

int main() {
    // create socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // define server address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(TCP_PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    // bind socket to specific IP and port
    int bind_status = bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address));
    if (bind_status == -1) {
        printf("Error binding to port %d!\n", TCP_PORT);
        exit(1);
    }

    // listen to socket
    int listen_status = listen(server_socket, BACKLOG);
    if (listen_status == -1) {
        printf("Error listening to port %d!\n", TCP_PORT);
        exit(1);
    }

    while(1) {
        printf("Listening for new connection to port %d...\n", TCP_PORT);
        pid_t pid;
        struct sockaddr_in client_address;
        socklen_t sin_size = sizeof(struct sockaddr_in);
        int client_socket = accept(server_socket, (struct sockaddr *) &client_address, &sin_size);
        
        if (client_socket == -1) {
            printf("Error in accepting client connection!\n");
            exit(1);
        }

        if ((pid = fork()) == 0) {
            close(server_socket);
            str_ser(client_socket);
            close(client_socket);
            exit(0);
        } else {
            close(client_socket);
        }
    }

    close(server_socket);
    exit(0);

    // char server_message[256] = "You have reached the server!";
    // // send message to client
    // int client_socket = accept(server_socket, NULL, NULL);
    // send(client_socket, server_message, sizeof(server_message), 0);

    // // close socket
    // close(server_socket);

    // return 0;
}

void str_ser(int sockfd) {
	char buf[BUFSIZE];
	FILE *fp;
	char recvs[DATALEN];
	struct ack_so ack;
	int end, n = 0;
	long lseek=0;
	end = 0;
    int packet_counter = 1;
	
	printf("Receiving data on port %d...\n", TCP_PORT);

	while(!end) {
        printf("Receiving packet #%d...\n", packet_counter);

        int ack_status;
		if ((n= recv(sockfd, &recvs, DATALEN, 0))==-1) {
			printf("Error receiving packet #%d!\n", packet_counter);
			exit(1);
		}

        // Send ACK
        ack.response = ack_num(ERROR_RATE);
	    ack.packet_no = packet_counter;
        if ((ack_status = send(sockfd, &ack, 2, 0)) == -1) {
			printf("Error sending acknolwedgement for packet #%d!\n", packet_counter);
			exit(1);
        }

        if (ack.response == 0) {
            printf("Received packet #%d!\n", packet_counter);
            if (recvs[n-1] == '\0') {
                end = 1;
                n--;
            }

            memcpy((buf+lseek), recvs, n);
            lseek += n;
            packet_counter++;
        } else {
            printf("Error found in packet #%d!\n", packet_counter);
        }
	}
	if ((fp = fopen ("myTCPreceive.txt","wt")) == NULL) {
		printf("File doesn't exit\n");
		exit(0);
	}
	fwrite (buf , 1 , lseek , fp);					//write data into file
	fclose(fp);
	printf("File successfully received!\nTotal size of data received is %d bytes\n", (int)lseek);
}

int ack_num(int error_rate) {
    int i = arc4random_uniform(100);

    if (i < error_rate) {
        return 1;  // Packet is errorneous -- return 1
    }
    return 0;   // Packet is not errorneous -- return 0
}