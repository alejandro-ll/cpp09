#include "RPN.hpp"
#include <iostream>

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Error: uso ./RPN \"expresion\"\n";
        return 1;
    }

    double result;
    if (RPN::evaluate(argv[1], result))
        std::cout << result << std::endl;
    else
        return 1;

    return 0;
}
