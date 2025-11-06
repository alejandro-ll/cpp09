#include "PmergeMe.hpp"
#include <algorithm>
#include <sstream>

bool PmergeMe::isAllDigits(const std::string& s) {
    if (s.empty()) return false;
    for (size_t i = 0; i < s.size(); ++i)
        if (s[i] < '0' || s[i] > '9') return false;
    return true;
}

bool PmergeMe::toIntChecked(const std::string& s, int& out) {
    // solo dígitos positivos (sin signo), rango 1..INT_MAX
    if (!isAllDigits(s)) return false;
    // convertir manual para evitar overflow
    long long acc = 0;
    for (size_t i = 0; i < s.size(); ++i) {
        acc = acc * 10 + (s[i] - '0');
        if (acc > INT_MAX) return false;
    }
    if (acc <= 0) return false;
    out = static_cast<int>(acc);
    return true;
}

size_t PmergeMe::parseArgs(int argc, char** argv,
                           std::vector<int>& outVec,
                           std::deque<int>& outDeq) {
    outVec.clear();
    outDeq.clear();

    for (int i = 1; i < argc; ++i) {
        std::string token(argv[i]);
        // permitir que vengan con espacios múltiples: ya están separados por el shell
        int val;
        if (!toIntChecked(token, val))
            throw std::runtime_error("Error: invalid token '" + token + "'");
        outVec.push_back(val);
        outDeq.push_back(val);
    }
    return outVec.size();
}

template <typename Cont>
void PmergeMe::buildPairsAndSort(Cont& input,
                                 std::vector< std::pair<int,int> >& pairs,
                                 int& stray,
                                 bool& hasStray) {
    pairs.clear();
    hasStray = false;

    // 1) formar pares (a,b) con a=menor, b=mayor
    size_t n = input.size();
    size_t i = 0;
    for (; i + 1 < n; i += 2) {
        int x = input[i];
        int y = input[i+1];
        if (x <= y) pairs.push_back(std::make_pair(x,y));
        else        pairs.push_back(std::make_pair(y,x));
    }
    if (i < n) {
        stray = input[i];
        hasStray = true;
    }

    // 2) ordenar los pares por su mayor (second)
    std::sort(pairs.begin(), pairs.end(),
        // comparar por el segundo elemento
        static_cast<bool (*)(const std::pair<int,int>&, const std::pair<int,int>&)>(
            // C++98: no lambdas; usamos un puntero a función estático
            // pero aquí mejor un functor in-situ:
            // truco: definimos una función local estática… no permitido en estándar;
            // así que implementamos comparador aquí mismo:
            0));
}

// Comparador estático global (C++98 no permite local functions fácilmente)
static bool cmpPairsBySecond(const std::pair<int,int>& a, const std::pair<int,int>& b) {
    return a.second < b.second;
}

// Re-implementamos con el comparador global
template <typename Cont>
void buildPairsAndSortWrapper(Cont& input,
                              std::vector< std::pair<int,int> >& pairs,
                              int& stray,
                              bool& hasStray) {
    pairs.clear();
    hasStray = false;
    size_t n = input.size();
    size_t i = 0;
    for (; i + 1 < n; i += 2) {
        int x = input[i];
        int y = input[i+1];
        if (x <= y) pairs.push_back(std::make_pair(x,y));
        else        pairs.push_back(std::make_pair(y,x));
    }
    if (i < n) { stray = input[i]; hasStray = true; }
    std::sort(pairs.begin(), pairs.end(), cmpPairsBySecond);
}

template <typename Cont>
void PmergeMe::binaryInsert(Cont& mainChain, int x) {
    // inserción binaria (lower_bound) en contenedor random-access (vector/deque)
    typename Cont::iterator it = std::lower_bound(mainChain.begin(), mainChain.end(), x);
    mainChain.insert(it, x);
}

// Genera el orden de inserción por índices (1..n-1) según Jacobsthal
void PmergeMe::buildJacobOrder(size_t n, std::vector<size_t>& order) {
    // n = número total de elementos "pendientes" (menores de cada par)
    // Secuencia de Jacobsthal: J0=0, J1=1, Jk=Jk-1 + 2*Jk-2
    // Orden típico: bloques decrecientes basados en Jk
    order.clear();
    if (n == 0) return;

    // construimos J hasta cubrir n
    std::vector<size_t> J;
    J.push_back(0); // J0
    J.push_back(1); // J1
    while (J.back() < n) {
        size_t sz = J.size();
        size_t next = J[sz-1] + 2 * J[sz-2];
        J.push_back(next);
        if (next == J.back()) break; // safety
    }

    // construir orden (1..n) pero agrupado por ventanas Jacobsthal
    // Insertamos índices 1..n siguiendo ventanas (Jk .. Jk-1+1) en orden descendente
    size_t prev = 1;
    for (size_t k = 2; k < J.size(); ++k) {
        size_t curr = J[k];
        if (curr > n) curr = n;
        // añadir [curr .. prev] decreciendo
        size_t i = curr;
        while (i >= prev) {
            order.push_back(i);
            if (i == prev) break;
            --i;
        }
        prev = curr + 1;
        if (curr == n) break;
    }
    // si quedaron índices (por si J no llegó exacto)
    for (size_t i = prev; i <= n; ++i) order.push_back(i);

    // Convertimos a 0-based (nuestros arrays de pend usan 0..n-1)
    for (size_t t = 0; t < order.size(); ++t) {
        if (order[t] > 0) order[t] -= 1;
        else order[t] = 0;
    }
}

template <typename Cont>
void PmergeMe::fordJohnsonSort(Cont& c) {
    const size_t N = c.size();
    if (N < 2) return;

    std::vector< std::pair<int,int> > pairs;
    int stray = 0; bool hasStray = false;

    // empaquetar y ordenar pares por el mayor
    buildPairsAndSortWrapper(c, pairs, stray, hasStray);

    // construir mainChain con los "mayores" y pend con los "menores"
    Cont mainChain;
    mainChain.resize(0);
    std::vector<int> pend;
    pend.reserve(pairs.size() + (hasStray ? 1 : 0));

    for (size_t i = 0; i < pairs.size(); ++i) {
        mainChain.push_back(pairs[i].second); // mayores ordenados
        pend.push_back(pairs[i].first);       // menores correspondientes
    }

    // insertar el primer 'pend' al principio según el algoritmo
    if (!pend.empty()) {
        binaryInsert(mainChain, pend[0]);
    }

    // construir orden Jacobsthal para el resto (si hay)
    if (pend.size() > 1) {
        std::vector<size_t> order;
        buildJacobOrder(pend.size() - 1, order); // ya insertamos el 0

        for (size_t oi = 0; oi < order.size(); ++oi) {
            size_t idx = order[oi] + 1; // desplazado porque metimos el 0
            if (idx < pend.size())
                binaryInsert(mainChain, pend[idx]);
        }
    }

    // si quedó suelto (tamaño impar), insertarlo también
    if (hasStray) {
        binaryInsert(mainChain, stray);
    }

    // copiar resultado a c
    // (mainChain ya está ordenado completamente)
    for (size_t i = 0; i < N; ++i) c[i] = mainChain[i];
}

// --- instanciaciones necesarias de las plantillas usadas en este .cpp ---
template void PmergeMe::fordJohnsonSort<std::vector<int> >(std::vector<int>&);
template void PmergeMe::fordJohnsonSort<std::deque<int> >(std::deque<int>&);
template void PmergeMe::binaryInsert<std::vector<int> >(std::vector<int>&, int);
template void PmergeMe::binaryInsert<std::deque<int> >(std::deque<int>&, int);
