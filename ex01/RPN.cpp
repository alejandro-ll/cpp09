#include "RPN.hpp"
#include <sstream>
#include <iostream>
#include <cstdlib>

bool RPN::evaluate(const std::string& expr, double& result) {
    std::istringstream iss(expr);
    std::stack<double> stack;
    std::string token;

    while (iss >> token) {
        // Si el token es un número
        if (std::isdigit(token[0]) || 
           ((token[0] == '-' || token[0] == '+') && token.size() > 1)) {
            stack.push(std::atof(token.c_str()));
        }
        // Si el token es un operador
        else if (token == "+" || token == "-" || token == "*" || token == "/") {
            if (stack.size() < 2) {
                std::cerr << "Error: expresión inválida\n";
                return false;
            }
            double b = stack.top(); stack.pop();
            double a = stack.top(); stack.pop();

            double res = 0;
            if (token == "+") res = a + b;
            else if (token == "-") res = a - b;
            else if (token == "*") res = a * b;
            else if (token == "/") {
                if (b == 0) {
                    std::cerr << "Error: división por cero\n";
                    return false;
                }
                res = a / b;
            }
            stack.push(res);
        }
        else {
            std::cerr << "Error: token inválido '" << token << "'\n";
            return false;
        }
    }

    if (stack.size() != 1) {
        std::cerr << "Error: expresión mal formada\n";
        return false;
    }

    result = stack.top();
    return true;
}
