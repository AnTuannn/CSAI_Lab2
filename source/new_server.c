#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <ctype.h>

#define MAX_SIZE 1024
#define PORT 8080
#define MAX_SIZE 100

// Include the calculateExpression function here
int stack[MAX_SIZE];
int top = -1;

void push(int x) {
    if (top == MAX_SIZE - 1) {
        printf("Stack overflow\n");
        return;
    }
    stack[++top] = x;
}

int pop() {
    if (top == -1) {
        printf("Stack underflow\n");
        return 0;
    }
    return stack[top--];
}

int precedence(char op) {
    switch (op) {
        case '+':
        case '-':
            return 1;
        case '*':
        case '/':
            return 2;
        default:
            return 0;
    }
}

int calculate(int num1, int num2, char op) {
     printf("%d %c %d = %d\n", num1, op, num2);
    switch (op) {
        case '+': return num1 + num2;
        case '-': return num1 - num2;
        case '*': return num1 * num2;
        case '/': return num1 / num2;
        default: return 0;
    }
   
}

int calculateExpression(const char* expression) {
    int i, num;
    char op;
    int numbers[MAX_SIZE];
    char operators[MAX_SIZE];
    int numTop = -1, opTop = -1;

    for (i = 0; i < strlen(expression); i++) {
        if (expression[i] == ' ') continue;

        if (isdigit(expression[i])) {
            num = 0;
            while (i < strlen(expression) && isdigit(expression[i])) {
                num = num * 10 + (expression[i] - '0');
                i++;
            }
            i--;
            numbers[++numTop] = num;
        } else if (expression[i] == '(') {
            operators[++opTop] = expression[i];
        } else if (expression[i] == ')') {
            while (opTop != -1 && operators[opTop] != '(') {
                num = calculate(numbers[numTop - 1], numbers[numTop], operators[opTop]);
                numbers[numTop - 1] = num;
                numTop--;
                opTop--;
            }
            opTop--; // remove '('
        } else {
            while (opTop != -1 && precedence(expression[i]) <= precedence(operators[opTop])) {
                num = calculate(numbers[numTop - 1], numbers[numTop], operators[opTop]);
                numbers[numTop - 1] = num;
                numTop--;
                opTop--;
            }
            operators[++opTop] = expression[i];
        }
    }

    while (opTop != -1) {
        num = calculate(numbers[numTop - 1], numbers[numTop], operators[opTop]);
        numbers[numTop - 1] = num;
        numTop--;
        opTop--;
    }

    return numbers[0];
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[MAX_SIZE] = {0};
    int valread;

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket to the network address and port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    while(1) {
        printf("\nWaiting for a connection...\n");

        // Accept a new connection
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        // Read the client's message
        valread = read(new_socket, buffer, MAX_SIZE);
        printf("Received: %s\n", buffer);

        // Calculate the expression
        int result = calculateExpression(buffer);
        sprintf(buffer, "%d", result);

        // Send the result back to the client
        send(new_socket, buffer, strlen(buffer), 0);
        printf("Result sent to client\n");

        close(new_socket);
    }

    return 0;
}