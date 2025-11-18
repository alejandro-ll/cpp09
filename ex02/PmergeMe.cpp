#include "PmergeMe.hpp"
#include <iomanip> // para setprecision

PmergeMe::PmergeMe() {}

PmergeMe::PmergeMe(const PmergeMe& other)
    : _vec(other._vec), _deq(other._deq) {}

PmergeMe& PmergeMe::operator=(const PmergeMe& other) {
    if (this != &other) {
        _vec = other._vec;
        _deq = other._deq;
    }
    return *this;
}

PmergeMe::~PmergeMe() {}

// -------------------- Helpers de parseo --------------------

bool PmergeMe::isValidNumber(const std::string& s) {
    if (s.empty())
        return false;

    std::size_t i = 0;
    if (s[0] == '+') {
        if (s.size() == 1)
            return false;
        i = 1;
    }

    for (; i < s.size(); ++i) {
        if (!std::isdigit(static_cast<unsigned char>(s[i])))
            return false;
    }
    return true;
}

long PmergeMe::stringToLong(const std::string& s) {
    std::istringstream iss(s);
    long value;
    iss >> value;
    if (iss.fail() || !iss.eof())
        throw std::runtime_error("invalid number");
    return value;
}

void PmergeMe::parseArguments(int argc, char** argv) {
    _vec.clear();
    _deq.clear();

    for (int i = 1; i < argc; ++i) {
        std::string token(argv[i]);

        if (!isValidNumber(token))
            throw std::runtime_error("invalid token");

        long value = stringToLong(token);

        if (value <= 0 || value > INT_MAX)
            throw std::runtime_error("out of range");

        int n = static_cast<int>(value);
        _vec.push_back(n);
        _deq.push_back(n);
    }

    if (_vec.empty())
        throw std::runtime_error("no numbers");
}

// -------------------- Sort para vector y deque --------------------

std::vector<int> PmergeMe::sortVector(const std::vector<int>& input) {
    return mergeInsertSort(input);
}

std::deque<int> PmergeMe::sortDeque(const std::deque<int>& input) {
    return mergeInsertSort(input);
}

// -------------------- Helpers de impresión --------------------

void PmergeMe::printContainer(const std::vector<int>& c) {
    for (std::vector<int>::const_iterator it = c.begin(); it != c.end(); ++it) {
        std::cout << *it;
        if (it + 1 != c.end())
            std::cout << " ";
    }
    std::cout << std::endl;
}

void PmergeMe::printContainer(const std::deque<int>& c) {
    for (std::deque<int>::const_iterator it = c.begin(); it != c.end(); ++it) {
        std::cout << *it;
        std::deque<int>::const_iterator tmp = it;
        ++tmp;
        if (tmp != c.end())
            std::cout << " ";
    }
    std::cout << std::endl;
}

// -------------------- Lógica principal (Before/After + tiempos) --------------------

void PmergeMe::process() {
    const std::size_t n = _vec.size();

    // Línea "Before"
    std::cout << "Before: ";
    printContainer(_vec);

    // Vector
    clock_t startVec = std::clock();
    std::vector<int> sortedVec = sortVector(_vec);
    clock_t endVec = std::clock();

    // Deque
    clock_t startDeq = std::clock();
    std::deque<int> sortedDeq = sortDeque(_deq);
    clock_t endDeq = std::clock();

    // Línea "After" (usamos el vector ordenado)
    std::cout << "After: ";
    printContainer(sortedVec);

    double timeVecUs = static_cast<double>(endVec - startVec)
                       * 1000000.0 / static_cast<double>(CLOCKS_PER_SEC);
    double timeDeqUs = static_cast<double>(endDeq - startDeq)
                       * 1000000.0 / static_cast<double>(CLOCKS_PER_SEC);

    std::cout << std::fixed << std::setprecision(5);
    std::cout << "Time to process a range of " << n
              << " elements with std::vector : " << timeVecUs
              << " us" << std::endl;
    std::cout << "Time to process a range of " << n
              << " elements with std::deque  : " << timeDeqUs
              << " us" << std::endl;
}
