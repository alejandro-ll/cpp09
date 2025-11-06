#ifndef PMERGEME_HPP
#define PMERGEME_HPP

#include <deque>
#include <vector>
#include <string>
#include <stdexcept>
#include <climits>

class PmergeMe {
public:
    // parsea argv a enteros positivos (1..INT_MAX) y devuelve el conteo
    static size_t parseArgs(int argc, char** argv,
                            std::vector<int>& outVec,
                            std::deque<int>& outDeq);

    // ordena usando Ford–Johnson (merge-insert) con inserciones en orden Jacobsthal
    template <typename Cont>
    static void fordJohnsonSort(Cont& c);

private:
    // utilidades internas
    static bool isAllDigits(const std::string& s);
    static bool toIntChecked(const std::string& s, int& out); // C++98 seguro

    template <typename Cont>
    static void binaryInsert(Cont& mainChain, int x);

    template <typename Cont>
    static void buildPairsAndSort(Cont& input,
                                  std::vector< std::pair<int,int> >& pairs,
                                  int& stray,
                                  bool& hasStray);

    static void buildJacobOrder(size_t n, std::vector<size_t>& order);
};

#endif
