#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    char filename[256], buffer[4096];
    struct sockaddr_in serv_addr;
    struct hostent *server;
    socklen_t serv_len;

    if (argc < 3) {
        printf("Usage: ./udp_file_client <hostname> <port>\n");
        exit(1);
    }

    portno = atoi(argv[2]);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    server = gethostbyname(argv[1]);
    if (server == NULL) {
        printf("No such host\n");
        exit(1);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    printf("Enter file name to request: ");
    scanf("%s", filename);

    serv_len = sizeof(serv_addr);

    // Send filename
    n = sendto(sockfd, filename, strlen(filename), 0,
               (struct sockaddr *)&serv_addr, serv_len);

    if (n < 0) {
        perror("Error sending filename");
        exit(1);
    }

    // Receive file contents
    memset(buffer, 0, sizeof(buffer));

    n = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0,
                 (struct sockaddr *)&serv_addr, &serv_len);

    if (n < 0) {
        perror("Error receiving file content");
        exit(1);
    }

    printf("\n--- File Content ---\n%s\n", buffer);

    close(sockfd);
    return 0;
}
