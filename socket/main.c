/*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int performCalculation(char* expression, double* result) {
    // Sử dụng atof để chuyển đổi chuỗi thành số thực
    *result = atof(expression);
    char operator;
    double operand;
    printf("%s\n", expression);

    // Lặp qua biểu thức để thực hiện các phép toán
    char* token = strtok(expression, "+-/");

    while (token != NULL) {
        int i = token - expression + strlen(token);
        operator = expression[token - expression + strlen(token)];
        token = strtok(NULL, "+-*");
        
        // Kiểm tra token để tránh NULL pointer
        if (token == NULL) {
            break;
        }
        printf("%s\n", expression);

        // Sử dụng biến tạm tempResult để lưu trữ giá trị trung gian
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

        // Gán giá trị mới cho biến result
        *result = tempResult;

        // Tìm token tiếp theo
        token = strtok(NULL, "+-*");
    }

    return 0;
}

int main() {
    double rs = 0;  // Khởi tạo giá trị cho rs
    char a[3] = "1+1";
    //// In giá trị rs trước khi gọi hàm performCalculation
    printf("Before: %f\n", rs);

    performCalculation(a, &rs);

    // In giá trị rs sau khi gọi hàm performCalculation
    printf("After: %f\n", rs);

  // Lấy ra toàn bộ token

    return 0;
} 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

double calculateExpression(char* expression) {
    char* expressionCopy = strdup(expression);  // Tạo bản sao của biểu thức
    if (expressionCopy == NULL) {
        printf("Error: Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    double result = 0;
    //printf("%f\n",result);
    char operator;
    double operand;
    // Sử dụng strtok để tách chuỗi
    char* token = strtok(expressionCopy, "+-*/()");
    result = atof(token);
    while (token != NULL) {
        operator = expression[token - expressionCopy + strlen(token)];
        token = strtok(NULL, "+-*/");

        // Kiểm tra token để tránh NULL pointer
        if (token == NULL) {
            break;
        }

        operand = atof(token);
        switch (operator) {
            case '+':
                result += operand;
                break;
            case '-':
                result -= operand;
                break;
            case '*':
                result *= operand;
                break;
            case '/':
                // Kiểm tra chia cho 0
                if (operand != 0) {
                    result /= operand;
                } else {
                    printf("Error: Division by zero\n");
                    free(expressionCopy);
                    exit(EXIT_FAILURE);
                }
                break;
            default:
                printf("Error: Invalid operator\n");
                free(expressionCopy);
                exit(EXIT_FAILURE);
        }

        // Tìm token tiếp theo
        //token = strtok(NULL, "+-*/");
    }

    free(expressionCopy);  // Giải phóng bộ nhớ của bản sao
    return result;
}

int main() {
    char* expression = "-3-1*3/2";
    double result = calculateExpression(expression);

    printf("Result: %f\n", result);

    return 0;
}
