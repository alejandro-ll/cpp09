#ifndef PMERGEME_HPP
#define PMERGEME_HPP

#include <vector>
#include <deque>
#include <string>
#include <stdexcept>
#include <climits>
#include <ctime>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <cctype>

// Clase principal: guarda la secuencia,
// la parsea desde argv y mide tiempos con vector y deque.
class PmergeMe {
public:
    PmergeMe();
    PmergeMe(const PmergeMe& other);
    PmergeMe& operator=(const PmergeMe& other);
    ~PmergeMe();

    // Lee y valida los argumentos, rellenando _vec y _deq
    void parseArguments(int argc, char** argv);

    // Lanza el sort en vector y deque, imprime Before/After y tiempos
    void process();

private:
    std::vector<int> _vec;
    std::deque<int>  _deq;

    // Helpers de parseo
    static bool isValidNumber(const std::string& s);
    static long stringToLong(const std::string& s);

    // Sort específico para cada contenedor
    static std::vector<int> sortVector(const std::vector<int>& input);
    static std::deque<int>  sortDeque(const std::deque<int>& input);

    // Helpers de impresión
    static void printContainer(const std::vector<int>& c);
    static void printContainer(const std::deque<int>& c);
};

// --------- Implementación del algoritmo genérico (template) ---------

// Merge-insert sort al estilo Ford-Johnson simplificado:
// 1. Formar parejas (max, min)
// 2. Ordenar las parejas por el max
// 3. Construir la "main chain" con los max
// 4. Insertar los min en la main chain usando búsqueda binaria
// 5. Insertar el "straggler" (elemento suelto) si lo hay
template <typename Cont>
Cont mergeInsertSort(const Cont& input) {
    typedef typename Cont::value_type T;
    typedef std::pair<T,T>           Pair;
    typedef std::vector<Pair>        PairVector;

    const std::size_t n = input.size();
    if (n <= 1)
        return input;

    PairVector pairs;
    pairs.reserve(n / 2);

    bool hasStraggler = false;
    T    straggler = T();

    // Formar las parejas (max, min)
    typename Cont::const_iterator it = input.begin();
    while (it != input.end()) {
        T first = *it;
        ++it;
        if (it != input.end()) {
            T second = *it;
            ++it;
            T big   = (first >= second) ? first : second;
            T small = (first <  second) ? first : second;
            pairs.push_back(std::make_pair(big, small));
        } else {
            // Elemento suelto si hay número impar de elementos
            hasStraggler = true;
            straggler    = first;
        }
    }

    // Ordenar parejas por el "max" (first)
    struct PairFirstLess {
        bool operator()(const Pair& a, const Pair& b) const {
            return a.first < b.first;
        }
    };

    std::sort(pairs.begin(), pairs.end(), PairFirstLess());

    // Construir la cadena principal (main chain) con los max
    Cont mainChain;
    typename PairVector::const_iterator pit = pairs.begin();
    for (; pit != pairs.end(); ++pit)
        mainChain.push_back(pit->first);

    // Pendientes: los min
    Cont pend;
    pit = pairs.begin();
    for (; pit != pairs.end(); ++pit)
        pend.push_back(pit->second);

    // Insertar el primer "min" en la main chain
    if (!pend.empty()) {
        typename Cont::value_type v0 = *pend.begin();
        typename Cont::iterator pos =
            std::lower_bound(mainChain.begin(), mainChain.end(), v0);
        mainChain.insert(pos, v0);

        // Insertar el resto de "min" uno a uno con búsqueda binaria
        typename Cont::iterator pendIt = pend.begin();
        ++pendIt; // el primero ya lo metimos
        for (; pendIt != pend.end(); ++pendIt) {
            typename Cont::iterator insertPos =
                std::lower_bound(mainChain.begin(), mainChain.end(), *pendIt);
            mainChain.insert(insertPos, *pendIt);
        }
    }

    // Insertar el straggler si lo hubiera
    if (hasStraggler) {
        typename Cont::iterator pos =
            std::lower_bound(mainChain.begin(), mainChain.end(), straggler);
        mainChain.insert(pos, straggler);
    }

    return mainChain;
}

#endif // PMERGEME_HPP
