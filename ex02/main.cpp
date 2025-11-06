#include "PmergeMe.hpp"
#include <iostream>
#include <iomanip>
#include <sys/time.h> // gettimeofday
#include <vector>
#include <deque>

static long long nowMicros() {
    timeval tv;
    gettimeofday(&tv, 0);
    return (long long)tv.tv_sec * 1000000LL + (long long)tv.tv_usec;
}

static void printLine(const char* label, const std::vector<int>& v, size_t max = 24) {
    std::cout << label << " ";
    size_t n = v.size();
    size_t limit = n <= max ? n : max;
    for (size_t i = 0; i < limit; ++i) std::cout << v[i] << " ";
    if (n > limit) std::cout << "[...]";
    std::cout << std::endl;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Error: usage: ./PmergeMe <positive int> <positive int> ..." << std::endl;
        return 1;
    }

    std::vector<int> v;
    std::deque<int>  d;
    size_t N = 0;

    try {
        N = PmergeMe::parseArgs(argc, argv, v, d);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    // Before
    printLine("Before:", v);

    // Vector timing
    std::vector<int> vCopy = v;
    long long t0 = nowMicros();
    PmergeMe::fordJohnsonSort(vCopy);
    long long t1 = nowMicros();

    // Deque timing
    std::deque<int> dCopy = d;
    long long t2 = nowMicros();
    PmergeMe::fordJohnsonSort(dCopy);
    long long t3 = nowMicros();

    // After
    printLine("After: ", vCopy);

    // Tiempos (us)
    std::cout << "Time to process a range of " << N
              << " elements with std::vector: " << (t1 - t0) << " us" << std::endl;

    std::cout << "Time to process a range of " << N
              << " elements with std::deque:  " << (t3 - t2) << " us" << std::endl;

    return 0;
}
