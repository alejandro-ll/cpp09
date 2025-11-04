#include "BitcoinExchange.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

static std::string trim(const std::string& s) {
    size_t i = 0, j = s.size();
    while (i < j && std::isspace(static_cast<unsigned char>(s[i]))) ++i;
    while (j > i && std::isspace(static_cast<unsigned char>(s[j-1]))) --j;
    return s.substr(i, j - i);
}

static bool splitPipe(const std::string& line, std::string& date, std::string& value) {
    size_t bar = line.find('|');
    if (bar == std::string::npos) return false;
    date = trim(line.substr(0, bar));
    value = trim(line.substr(bar+1));
    return true;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Error: could not open file." << std::endl;
        return 1;
    }

    // Carga de la BD de tipos de cambio
    BitcoinExchange bex;
    try {
        bex.loadRates("data.csv");
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    if (!bex.hasRates()) {
        std::cout << "Error: empty or invalid data.csv." << std::endl;
        return 1;
    }

    std::ifstream in(argv[1]);
    if (!in) {
        std::cout << "Error: could not open file." << std::endl;
        return 1;
    }

    std::string line;
    // Posible cabecera "date | value"
    if (std::getline(in, line)) {
        std::string t = trim(line);
        if (t != "date | value") {
            // no era cabecera; procesamos esa primera línea
            in.seekg(0);
        }
    }

    while (std::getline(in, line)) {
        if (trim(line).empty()) continue;
        std::string date, valueStr;
        if (!splitPipe(line, date, valueStr)) {
            std::cout << "Error: bad input => " << line << std::endl;
            continue;
        }
        if (!BitcoinExchange::isValidDate(date)) {
            std::cout << "Error: bad input => " << line << std::endl;
            continue;
        }
        double value;
        if (!BitcoinExchange::parseNumber(valueStr, value)) {
            std::cout << "Error: bad input => " << line << std::endl;
            continue;
        }
        if (value < 0) {
            std::cout << "Error: not a positive number." << std::endl;
            continue;
        }
        if (value > 1000.0) {
            std::cout << "Error: too large a number." << std::endl;
            continue;
        }

        double rate;
        if (!bex.getRateForDate(date, rate)) {
            std::cout << "Error: no exchange rate available for date <= " << date << std::endl;
            continue;
        }
        double result = value * rate;
        std::cout << date << " => " << value << " = " << result << std::endl;
    }
    return 0;
}
