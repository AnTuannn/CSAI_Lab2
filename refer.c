#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void removeSpaces(char *input) {
    int i, j = 0;
    for (i = 0; input[i] != '\0'; i++) {
        if (input[i] != ' ') {
            input[j++] = input[i];
        }
    }
    input[j] = '\0';
}

int calculate(int *numbers, char *operators, int length) {
    // Handle multiplication and division first
    for (int i = 0; i < length; i++) {
        if (operators[i] == '*' || operators[i] == '/') {
            int result;
            switch (operators[i]) {
                case '*':
                    result = numbers[i] * numbers[i + 1];
                    break;
                case '/':
                    // Check if division by zero
                    if (numbers[i + 1] != 0) {
                        result = numbers[i] / numbers[i + 1];
                    } else {
                        printf("Error: Division by zero\n");
                        return 0;
                    }
                    break;
            }
            // Adjust numbers and operators arrays after calculation
            for (int j = i; j < length - 1; j++) {
                numbers[j] = numbers[j + 1];
                operators[j] = operators[j + 1];
            }
            numbers[i] = result;
            length -= 1;
            i -= 1; // Go back one step to re-evaluate from the current index
        }
    }

    // Then handle addition and subtraction
    for (int i = 0; i < length - 1; i++) {
        switch (operators[i]) {
            case '+':
                numbers[i + 1] = numbers[i] + numbers[i + 1];
                break;
            case '-':
                numbers[i + 1] = numbers[i] - numbers[i + 1];
                break;
            default:
                // Handle invalid operator
                printf("Error: Invalid operator\n");
                return 0;
        }
    }
    return numbers[length - 1];
}

int evaluateInParentheses(char *input, int start, int end) {
    int numbers[50]; // Assuming maximum 50 numbers
    char operators[50]; // Assuming maximum 50 operators
    char temp[20]; // Temporary storage for a number
    int numCount = 0, opCount = 0;
    int i = start;

    while(i<end){
        if (input[i] == '(') {
            int j = i + 1;
            int stack = 1;

            while (stack != 0) {
                if (input[j] == '(') {
                    stack++;
                } else if (input[j] == ')') {
                    stack--;
                }
                j++;
            }

            numbers[numCount++] = evaluateInParentheses(input, i + 1, j - 1);
            i = j;
        }
        else if (input[i] == '+' || input[i] == '-'|| input[i] == '*'|| input[i] == '/') {
            operators[opCount++] = input[i++];
        } 
        else if (input[i] >= '0' && input[i] <= '9') {
            int index = 0;
            temp[index++] = input[i++];
            while (input[i] >= '0' && input[i] <= '9') {
                temp[index++] = input[i++];
            }
            temp[index++] = '\0';
            numbers[numCount++] = atoi(temp);
        }
        else {
            i++;
        }
        
    }
    operators[opCount] = '\0';
    return calculate(numbers, operators, opCount + 1);
}

void splitInput(char *input, int *numbers, char *operators) {
    removeSpaces(input);

    int numCount = 0, opCount = 0;
    char temp[20]; // Temporary storage for a number
    int i = 0;

    while (input[i] != '\0') {
        if (input[i] == '(') {
            int j = i + 1;
            int stack = 1;

            while (stack != 0) {
                if (input[j] == '(') {
                    stack++;
                } else if (input[j] == ')') {
                    stack--;
                }
                j++;
            }
            int ducnam_temp = evaluateInParentheses(input, i + 1, j - 1);
            // printf("%d",ducnam_temp);
            numbers[numCount++] = ducnam_temp;
            i = j;
        } else if (input[i] == '+' || input[i] == '-'|| input[i] == '*'|| input[i] == '/') {
            operators[opCount++] = input[i++];
        } else if (input[i] >= '0' && input[i] <= '9') {
            int index = 0;
            temp[index++] = input[i++];
            while (input[i] >= '0' && input[i] <= '9') {
                temp[index++] = input[i++];
            }
            temp[index++] = '\0';
            numbers[numCount++] = atoi(temp);
        } else {
            i++;
        }
    }
    operators[opCount] = '\0';
}

int main() {
    char input[100];
    int numbers[50]; // Assuming maximum 50 numbers
    char operators[50]; // Assuming maximum 50 operators

    printf("Enter the expression (e.g., 2+(3-1)): ");
    fgets(input, sizeof(input), stdin);

    input[strcspn(input, "\n")] = 0; // Removing the newline character

    splitInput(input, numbers, operators);

    int result = calculate(numbers, operators, strlen(operators) + 1);
    printf("Result: %d\n", result);

    return 0;
}
