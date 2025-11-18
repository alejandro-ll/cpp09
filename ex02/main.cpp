#include "PmergeMe.hpp"
#include <iostream>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Error" << std::endl;
        return 1;
    }

    try {
        PmergeMe sorter;
        sorter.parseArguments(argc, argv);
        sorter.process();
    } catch (const std::exception&) {
        // El subject solo exige un mensaje de error en stderr.
        // Para estar alineados con los ejemplos, imprimimos "Error".
        std::cerr << "Error" << std::endl;
        return 1;
    }

    return 0;
}
