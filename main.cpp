#include <iostream>

int main() {
    char op;
    double num1, num2;

    std::cout << "Enter an expression (e.g., 5 + 55): ";

    std::cin >> num1 >> op >> num2;

    switch (op) {
    case '+':
        std::cout << num1 << " + " << num2 << " = " << num1 + num2 << std::endl;
        break;

    case '-':
        std::cout << num1 << " - " << num2 << " = " << num1 - num2 << std::endl;
        break;

    case '*':
        std::cout << num1 << " * " << num2 << " = " << num1 * num2 << std::endl;
        break;

    case '/':
        if (num2 != 0) {
            std::cout << num1 << " / " << num2 << " = " << num1 / num2 << std::endl;
        }
        else {
            std::cerr << "Error: Division by zero is not allowed." << std::endl;
        }
        break;

    default:
        std::cerr << "Error: Invalid operator or format. Please use the format: number operator number" << std::endl;
        break;
    }

    return 0;
}
