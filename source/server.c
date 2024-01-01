#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#define PORT 8000
#define MAX_BUFFER_SIZE 256
#define MAX_SIZE 100

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
    switch (op) {
        case '+': return num1 + num2;
        case '-': return num1 - num2;
        case '*': return num1 * num2;
        case '/': return num1 / num2;
        default: return 0;
    }
   
}
int checkExpression(const char* expression)
{
    for (int i = 0; i < strlen(expression); i++)
    {
        if (expression[i] != '(' && expression[i] != ')' && 
            expression[i] != '+' && expression[i] != '-' && 
            expression[i] != '*' && expression[i] != '/' && 
            expression[i] != ' ' && !isdigit(expression[i])) {
                return i+1;
            }
    }
    return 0;
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
        } 
        else if (expression[i] == '(') {
            operators[++opTop] = expression[i];
        } 
        else if (expression[i] == ')') {
            while (opTop != -1 && operators[opTop] != '(') {
                num = calculate(numbers[numTop - 1], numbers[numTop], operators[opTop]);
                numbers[numTop - 1] = num;
                numTop--;
                opTop--;
            }
            opTop--; // remove '('
        } 
        else if (!isdigit(expression[i]) && expression[i] != '+' && expression[i] != '-' && expression[i] != '*' && expression[i] != '/' && expression[i] != '(' && expression[i] != ')') {
            return false;
        } 
        else {
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


// This function will handle connection for each client
void *handle_client(void *arg) {
    int clientSocket = *((int *)arg);
    free(arg);

    while(1) {
        char buffer[MAX_BUFFER_SIZE];
        ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

        if (bytesRead <= 0) {
            perror("Error receiving data or client disconnected");
            close(clientSocket);
            pthread_exit(NULL);
        }

        buffer[bytesRead] = '\0';
        if (strcmp(buffer, "exit") == 0) {
            printf("Client disconnected\n");
            close(clientSocket);
            pthread_exit(NULL);
        }
        int check = checkExpression(buffer);
        int result = 0;
        if (check != 0)
        {
            result = check;
            sprintf(buffer, "Error: Expression has an error at the %d character, not a number or a mathematical operator, please check again.", result);
        }
        else
        {
            result = calculateExpression(buffer);
            sprintf(buffer, "%d", result);
        }
        
        send(clientSocket, buffer, strlen(buffer), 0);
    }
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
    while(1) {
        int *new_sock;
        new_sock = malloc(1);
        *new_sock = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrLen);

        if (*new_sock < 0) {
            perror("Error accepting connection");
            exit(EXIT_FAILURE);
        }

        printf("Connection accepted from %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

        pthread_t client_thread;
        if (pthread_create(&client_thread, NULL, handle_client, (void*) new_sock) < 0) {
            perror("Error creating thread");
            return 1;
        }

        // Now join the thread, so that we don't terminate before the thread
        pthread_join(client_thread, NULL);
        printf("Handler assigned\n");
    }
    // Đóng kết nối
    close(serverSocket);

    return 0;
}
