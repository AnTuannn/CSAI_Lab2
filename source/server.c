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

// Hàm để push value vào stack
void push(int x) 
{
    if (top == MAX_SIZE - 1) 
    {
        printf("Stack overflow\n");
        return;
    }
    stack[++top] = x;
}
// Hàm để pop value ra khỏi stack
int pop() 
{
    if (top == -1) 
    {
        printf("Stack underflow\n");
        return 0;
    }
    return stack[top--];
}

// Hàm để xác định độ ưu tiên của toán tử với giá trị càng lớn độ ưu tiên càng cao
int precedence(char op) 
{
    switch (op) 
    {
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

// Hàm để tính toán biểu thức với 2 số và 1 toán tử
int calculate(int num1, int num2, char op) 
{
    switch (op) 
    {
        case '+': return num1 + num2;
        case '-': return num1 - num2;
        case '*': return num1 * num2;
        case '/': return num1 / num2;
        default: return 0;
    }
   
}

// Hàm để kiểm tra biểu thức nhập vào có đúng cú pháp không
int checkExpression(const char* expression)
{
    int i;
    for (i = 0; i < strlen(expression); i++)
    {
        if (expression[i] != '(' && expression[i] != ')' && 
            expression[i] != '+' && expression[i] != '-' && 
            expression[i] != '*' && expression[i] != '/' && expression[i] != ')' && 
            expression[i] != ' ' && !isdigit(expression[i])) 
            {
                return i+1;
            }
    }
    return 0;
}

//Hàm để tính toán và trả về kết quả của biểu thức được đưa vào
int calculateExpression(const char* expression) 
{
    int i, num; // num là số được đọc từ biểu thức, i là biến đếm
    char op; // op là toán tử được đọc từ biểu thức + - * /
    int numbers[MAX_SIZE]; // numbers là mảng chứa các số được đọc từ biểu thức
    char operators[MAX_SIZE]; // operators là mảng chứa các toán tử được đọc từ biểu thức
    int numTop = -1, opTop = -1; // numTop và opTop là biến đếm số lượng phần tử trong mảng numbers và operators

    // Đọc từng ký tự trong biểu thức
    for (i = 0; i < strlen(expression); i++) 
    {
        if (expression[i] == ' ') continue; // Bỏ qua khoảng trắng 

        if (isdigit(expression[i])) //Nếu là số
        {
            num = 0;
            while (i < strlen(expression) && isdigit(expression[i])) // Đọc từng ký tự trong số
            {
                num = num * 10 + (expression[i] - '0'); // Chuyển ký tự thành số
                i++;
            }
            i--;
            numbers[++numTop] = num; // Thêm số vào mảng numbers
        } 
        else if (expression[i] == '(')  // Nếu là dấu mở ngoặc
        {
            operators[++opTop] = expression[i]; // Thêm dấu mở ngoặc vào mảng operators
        } 
        else if (expression[i] == ')') // Nếu là dấu đóng ngoặc
        {
            while (opTop != -1 && operators[opTop] != '(') // Nếu phần tử cuối cùng trong mảng operators không phải là dấu mở ngoặc
            {
                num = calculate(numbers[numTop - 1], numbers[numTop], operators[opTop]); // Tính toán với 2 số và 1 toán tử
                numbers[numTop - 1] = num; // Thay thế số thứ 2 bằng kết quả tính toán
                numTop--;
                opTop--;
            }
            opTop--; // Xóa dấu mở ngoặc
        } 
        else if (!isdigit(expression[i]) && expression[i] != '+' &&  
                    expression[i] != '-' && expression[i] != '*' && 
                    expression[i] != '/' && expression[i] != '(' && 
                    expression[i] != ')')  
            return 0;
        else 
        {
            while (opTop != -1 && precedence(expression[i]) <= precedence(operators[opTop])) // Nếu độ ưu tiên của toán tử hiện tại nhỏ hơn hoặc bằng độ ưu tiên của toán tử cuối cùng trong mảng operators
            {
                num = calculate(numbers[numTop - 1], numbers[numTop], operators[opTop]); // Tính toán với 2 số và 1 toán tử
                numbers[numTop - 1] = num; // Thay thế số thứ 2 bằng kết quả tính toán
                numTop--;
                opTop--;
            }
            operators[++opTop] = expression[i];
        }   
    }

    // Tính toán các phép toán còn lại trong mảng operators và numbers
    while (opTop != -1) 
    {
        num = calculate(numbers[numTop - 1], numbers[numTop], operators[opTop]);
        numbers[numTop - 1] = num;
        numTop--;
        opTop--;
    }

    return numbers[0]; // Trả về kết quả
}


// Hàm dùng để xử lý tín hiệu của từng client
void *handle_client(void *arg) 
{
    int clientSocket = *((int *)arg); // Tạo socket cho client đầu vào
    free(arg);

    while(1) 
    {
        char buffer[MAX_BUFFER_SIZE];
        ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0); // Đọc dữ liệu từ client

        buffer[bytesRead] = '\0';
        // Kiểm tra nếu client muốn thoát
        if (strcmp(buffer, "exit") == 0) 
        {
            printf("Client %d disconnected\n", clientSocket);
            close(clientSocket); // Đóng kết nối
            pthread_exit(NULL);
        }

        if (bytesRead <= 0) 
        {
            perror("Error receiving data or client disconnected");
            close(clientSocket);
            pthread_exit(NULL);
        }

        int check = checkExpression(buffer); // Kiểm tra biểu thức nhập vào có đúng cú pháp không
        int result = 0;
        if (check != 0)
        {
            result = check;
            sprintf(buffer, "Error at character %d, check again!", result); // Gửi thông báo lỗi đến client
        }
        else
        {
            result = calculateExpression(buffer);
            sprintf(buffer, "%d", result);
        }
        
        send(clientSocket, buffer, strlen(buffer), 0); // Gửi kết quả tính toán đến client
    }
}

int main() 
{
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrLen = sizeof(clientAddr);

    // Tạo socket
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
    {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Thiết lập địa chỉ server
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    // Liên kết địa chỉ với socket
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) 
    {
        perror("Error binding");
        exit(EXIT_FAILURE);
    }

    // Lắng nghe kết nối từ client
    if (listen(serverSocket, 1) == -1) 
    {
        perror("Error listening");
        exit(EXIT_FAILURE);
    }
    printf("Server listening on port %d...\n", PORT);

    // Xử lý tín hiệu từ nhiều client
    while(1) 
    {
        int *new_sock; // Tạo socket cho client mới
        new_sock = malloc(1); // Cấp phát bộ nhớ cho socket
        *new_sock = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrLen); // Chấp nhận kết nối từ client

        if (*new_sock < 0) {
            perror("Error accepting connection");
            exit(EXIT_FAILURE);
        }

        printf("Connection accepted from %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

        pthread_t client_thread;

        // Tạo thread để xử lý tín hiệu của client
        if (pthread_create(&client_thread, NULL, handle_client, (void*) new_sock) < 0) {
            perror("Error creating thread");
            return 1;
        }

        //Thông báo thread được tạo thành công
        printf("Handler assigned\n");
        printf("Waiting for new connection...\n");
    }
    // Đóng kết nối
    close(serverSocket);

    return 0;
}
