#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    char filename[256], buffer[4096], line[512];
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t cli_len;
    FILE *fp;

    if (argc < 2) {
        printf("Usage: ./udp_file_server <port>\n");
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    memset(&cli_addr, 0, sizeof(cli_addr));

    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Bind failed");
        exit(1);
    }

    printf("UDP File Server running... waiting for filename...\n");

    cli_len = sizeof(cli_addr);

    // Receive filename
    memset(filename, 0, sizeof(filename));
    n = recvfrom(sockfd, filename, sizeof(filename) - 1, 0,
                 (struct sockaddr *)&cli_addr, &cli_len);

    if (n < 0) {
        perror("Error receiving filename");
        exit(1);
    }

    printf("Client requested file: %s\n", filename);

    // Try opening the file
    fp = fopen(filename, "r");
    if (fp == NULL) {
        strcpy(buffer, "File not found");
        sendto(sockfd, buffer, strlen(buffer), 0,
               (struct sockaddr *)&cli_addr, cli_len);
        printf("File not found. Sent error message.\n");
        close(sockfd);
        return 0;
    }

    // Read file content
    memset(buffer, 0, sizeof(buffer));
    while (fgets(line, sizeof(line), fp) != NULL) {
        strcat(buffer, line);
    }
    fclose(fp);

    // Send file contents
    n = sendto(sockfd, buffer, strlen(buffer), 0,
               (struct sockaddr *)&cli_addr, cli_len);

    if (n < 0)
        perror("Error sending file contents");

    printf("File content sent successfully.\n");

    close(sockfd);
    return 0;
}
