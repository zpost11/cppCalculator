#include <iostream>

int main() {
    char op;
    double num1, num2;

    std::cout << "Enter an operator (+, -, *, /): ";
    std::cin >> op;

    std::cout << "Enter two numbers: ";
    std::cin >> num1 >> num2;

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
            // Check for division by zero
            if (num2 != 0) {
                std::cout << num1 << " / " << num2 << " = " << num1 / num2 << std::endl;
            } else {
                std::cerr << "Error: Division by zero is not allowed." << std::endl;
            }
            break;

        default:
            //Ensure all cases are handled
            std::cerr << "Error: Invalid operator. Please use +, -, *, or /." << std::endl;
            break;
    }

    return 0;
}
