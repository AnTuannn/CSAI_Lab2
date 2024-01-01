#include <iostream>
#include <sstream>
#include <stack>
#include <cctype>

double calculateExpression(const char* expression) {
    std::istringstream iss(expression);
    std::stack<double> numbers;
    std::stack<char> operations;

    char current;
    double operand;

    while (iss >> std::ws >> current) {
        if (isdigit(current) || current == '.') {
            iss.putback(current);
            iss >> operand;
            numbers.push(operand);
        } else if (current == '(') {
            operations.push(current);
        } else if (current == ')' && !operations.empty()) {
            while (operations.top() != '(') {
                char op = operations.top();
                operations.pop();

                double operand2 = numbers.top();
                numbers.pop();

                double operand1 = numbers.top();
                numbers.pop();

                switch (op) {
                    case '+':
                        numbers.push(operand1 + operand2);
                        break;
                    case '-':
                        numbers.push(operand1 - operand2);
                        break;
                    case '*':
                        numbers.push(operand1 * operand2);
                        break;
                    case '/':
                        if (operand2 != 0.0) {
                            numbers.push(operand1 / operand2);
                        } else {
                            std::cerr << "Error: Division by zero\n";
                            return 0.0; // hoặc giá trị mặc định khác tùy ý
                        }
                        break;
                }
            }
            operations.pop(); // Loại bỏ dấu '('
        } else if (current == '+' || current == '-' || current == '*' || current == '/') {
            while (!operations.empty() && (current == '*' || current == '/') && (operations.top() == '*' || operations.top() == '/')) {
                char op = operations.top();
                operations.pop();

                double operand2 = numbers.top();
                numbers.pop();

                double operand1 = numbers.top();
                numbers.pop();

                switch (op) {
                    case '*':
                        numbers.push(operand1 * operand2);
                        break;
                    case '/':
                        if (operand2 != 0.0) {
                            numbers.push(operand1 / operand2);
                        } else {
                            std::cerr << "Error: Division by zero\n";
                            return 0.0; // hoặc giá trị mặc định khác tùy ý
                        }
                        break;
                }
            }
            operations.push(current);
        }
    }

    while (!operations.empty()) {
        char op = operations.top();
        operations.pop();

        double operand2 = numbers.top();
        numbers.pop();

        double operand1 = numbers.top();
        numbers.pop();

        switch (op) {
            case '+':
                numbers.push(operand1 + operand2);
                break;
            case '-':
                numbers.push(operand1 - operand2);
                break;
            case '*':
                numbers.push(operand1 * operand2);
                break;
            case '/':
                if (operand2 != 0.0) {
                    numbers.push(operand1 / operand2);
                } else {
                    std::cerr << "Error: Division by zero\n";
                    return 0.0; // hoặc giá trị mặc định khác tùy ý
                }
                break;
        }
    }

    if (!numbers.empty()) {
        return numbers.top();
    }

    return 0.0;
}

int main() {
    const char* expression = "(1 + 2) * 3 - 4";

    double result = calculateExpression(expression);

    std::cout << "Result: " << result << std::endl;

    return 0;
}
