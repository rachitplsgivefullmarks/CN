#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno, len, n;
    char buffer[256], c[2000], cc[20000];
    struct sockaddr_in serv, cli;
    FILE *fd;

    if (argc < 2) {
        printf("Error: no port number provided.\nUsage: ./server <portno>\nExample: ./server 7777\n");
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error opening socket");
        exit(1);
    }

    portno = atoi(argv[1]);
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = INADDR_ANY;
    serv.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *)&serv, sizeof(serv)) < 0) {
        perror("Error on binding");
        exit(1);
    }

    listen(sockfd, 10);
    len = sizeof(cli);
    printf("Server: waiting for connection...\n");

    newsockfd = accept(sockfd, (struct sockaddr *)&cli, (socklen_t *)&len);
    if (newsockfd < 0) {
        perror("Error on accept");
        exit(1);
    }

    bzero(buffer, 255);
    n = read(newsockfd, buffer, 255);
    if (n < 0) {
        perror("Error reading from socket");
        exit(1);
    }

    printf("Server received: %s\n", buffer);

    if ((fd = fopen(buffer, "r")) != NULL) {
        printf("Server: %s found. Opening and reading...\n", buffer);
        bzero(cc, sizeof(cc));

        while (fgets(c, sizeof(c), fd) != NULL) {
            strcat(cc, c);
        }
        fclose(fd);

        n = write(newsockfd, cc, strlen(cc));
        if (n < 0)
            perror("Error writing to socket");
        else
            printf("Transfer complete.\n");
    } else {
        printf("Server: file not found.\n");
        n = write(newsockfd, "File not found", 15);
        if (n < 0)
            perror("Error writing to socket");
    }

    close(newsockfd);
    close(sockfd);
    return 0;
}
