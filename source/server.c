#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080
#define MAX_BUFFER_SIZE 256

int performCalculation(char* expression, double* result) {
     // Sử dụng atof để chuyển đổi chuỗi thành số thực
    result = atof(expression);
    char operator;
    double operand;

    // Lặp qua biểu thức để thực hiện các phép toán
    char* token = strtok(expression, "+-*/");
    while (token != NULL) {
        operator = token[strlen(token) - 1];
        token = strtok(NULL, "+-*/");
        operand = atof(token);
        double tempResult = *result;

        switch (operator) {
            case '+':
                tempResult += operand;
                break;
            case '-':
                tempResult -= operand;
                break;
            case '*':
                tempResult *= operand;
                break;
            case '/':
                // Kiểm tra chia cho 0
                if (operand != 0) {
                    tempResult /= operand;
                } else {
                    printf("Error: Division by zero\n");
                    return -1;
                }
                break;
            default:
                printf("Error: Invalid operator\n");
                return -1;
        }
        *result = tempResult
        // Tìm token tiếp theo
        token = strtok(NULL, "+-*/");
    }
    return 0;
}

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrLen = sizeof(clientAddr);

    // Tạo socket
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Thiết lập địa chỉ server
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    // Liên kết địa chỉ với socket
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Error binding");
        exit(EXIT_FAILURE);
    }

    // Lắng nghe kết nối từ client
    if (listen(serverSocket, 1) == -1) {
        perror("Error listening");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    // Chấp nhận kết nối từ client
    if ((clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrLen)) == -1) {
        perror("Error accepting connection");
        exit(EXIT_FAILURE);
    }

    printf("Connection accepted from %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

    // Nhận dữ liệu từ client
    char buffer[MAX_BUFFER_SIZE];
    ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

    if (bytesRead == -1) {
        perror("Error receiving data");
        exit(EXIT_FAILURE);
    }

    buffer[bytesRead] = '\0';

    // Thực hiện tính toán
    double result;
    if (performCalculation(buffer, &result) == 0) {
        // Gửi kết quả về cho client
        char resultStr[MAX_BUFFER_SIZE];
        snprintf(resultStr, sizeof(resultStr), "%.2f", result);
        send(clientSocket, resultStr, strlen(resultStr), 0);
    } else {
        // Gửi thông báo lỗi về cho client
        char errorMsg[] = "Invalid syntax";
        send(clientSocket, errorMsg, strlen(errorMsg), 0);
    }

    // Đóng kết nối
    close(clientSocket);
    close(serverSocket);

    return 0;
}
