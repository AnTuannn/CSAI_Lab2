#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define PORT 8080
#define MAX_SIZE 1024

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[MAX_SIZE] = {0};

    // Create a socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    // Send an expression to the server
    char expression[MAX_SIZE];
    printf("Enter the expression (type 'exit' to stop): ");
    fgets(expression, MAX_SIZE, stdin);

    // Check if the user wants to exit
    if (strcmp(expression, "exit\n") == 0) {
        printf("Exiting...\n");
        return 0;
    }

    send(sock, expression, strlen(expression), 0);
    printf("Expression sent to server\n");

    // Receive the result from the server
    read(sock, buffer, MAX_SIZE);
    printf("Result from server: %s\n", buffer);

    return 0;
}