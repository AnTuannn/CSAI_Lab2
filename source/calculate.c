#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    char expression[] = "(2*3 +5 *7 - 2)*5 + 3*2 + (12+3-7)";
    printf("Result: %d\n", calculateExpression(expression));
    return 0;
}