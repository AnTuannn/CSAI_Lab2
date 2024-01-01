#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>

#define PORT 8000
#define MAX_BUFFER_SIZE 256

int main() {
    int clientSocket;
    struct sockaddr_in serverAddr;

    // Tạo socket
    if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Thiết lập địa chỉ server
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);

    // Chuyển đổi địa chỉ IP sang địa chỉ máy chủ
    if (inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    // Kết nối đến server
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Error connecting to server");
        exit(EXIT_FAILURE);
    }
    while(1) {
        // Nhập dữ liệu từ người dùng
        printf("Enter an arithmetic expression (or 'exit' to quit): ");
        char expression[MAX_BUFFER_SIZE];
        fgets(expression, sizeof(expression), stdin);

        // Remove newline character at the end of the expression
        expression[strcspn(expression, "\n")] = 0;

        // Check if the user wants to exit
        if (strcmp(expression, "exit") == 0) {
            break; // Break the loop
        }

        // Gửi dữ liệu đến server
        send(clientSocket, expression, strlen(expression), 0);

        // Nhận kết quả từ server
        char buffer[MAX_BUFFER_SIZE];
        ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

        if (bytesRead == -1) {
            perror("Error receiving data");
            continue; // Skip the rest of this loop iteration
        }

        // Null-terminate the received data to safely use it as a string
        buffer[bytesRead] = '\0';

        // Print the result
        printf("Result: %s\n", buffer);
    }
    
    // Đóng kết nối
    close(clientSocket);

    return 0;
}
