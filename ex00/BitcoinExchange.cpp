#include "BitcoinExchange.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cctype>
#include <cstdlib>

BitcoinExchange::BitcoinExchange() {}

static std::string trim(const std::string& s) {
    size_t i = 0, j = s.size();
    while (i < j && std::isspace(static_cast<unsigned char>(s[i]))) ++i;
    while (j > i && std::isspace(static_cast<unsigned char>(s[j-1]))) --j;
    return s.substr(i, j - i);
}

void BitcoinExchange::loadRates(const std::string& csvPath) {
    std::ifstream f(csvPath.c_str());
    if (!f) throw std::runtime_error("Error: could not open data.csv.");
    std::string line;
    // cabecera esperada: "date,exchange_rate"
    std::getline(f, line);
    while (std::getline(f, line)) {
        if (line.empty()) continue;
        std::istringstream ss(line);
        std::string date, priceStr;
        if (!std::getline(ss, date, ',')) continue;
        if (!std::getline(ss, priceStr)) continue;
        date = trim(date);
        priceStr = trim(priceStr);
        if (!isValidDate(date)) continue;
        double price;
        std::istringstream pss(priceStr);
        if (!(pss >> price)) continue;
        _rates[date] = price;
    }
}

bool BitcoinExchange::hasRates() const {
    return !_rates.empty();
}

bool BitcoinExchange::getRateForDate(const std::string& date, double& rate) const {
    // exacta
    std::map<std::string,double>::const_iterator it = _rates.find(date);
    if (it != _rates.end()) { rate = it->second; return true; }
    // más cercana inferior
    it = _rates.upper_bound(date);
    if (it == _rates.begin()) return false;
    --it;
    rate = it->second;
    return true;
}

static bool isLeap(int y){ return (y%4==0 && y%100!=0) || (y%400==0); }

bool BitcoinExchange::isValidDate(const std::string& d) {
    // Formato YYYY-MM-DD con rangos válidos
    if (d.size() != 10 || d[4] != '-' || d[7] != '-') return false;
    for (size_t i=0;i<d.size();++i){
        if (i==4 || i==7) continue;
        if (!std::isdigit(static_cast<unsigned char>(d[i]))) return false;
    }
    int y = std::atoi(d.substr(0,4).c_str());
    int m = std::atoi(d.substr(5,2).c_str());
    int day = std::atoi(d.substr(8,2).c_str());
    if (m < 1 || m > 12) return false;
    static const int mdays[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
    int maxd = mdays[m-1];
    if (m==2 && isLeap(y)) maxd = 29;
    if (day < 1 || day > maxd) return false;
    return true;
}

bool BitcoinExchange::parseNumber(const std::string& s, double& out) {
    // admite enteros/float (+espacios), sin notación científica
    std::string t = trim(s);
    if (t.empty()) return false;
    // valida formato básico
    bool dotSeen=false; size_t i=0;
    if (t[0]=='+' || t[0]=='-') i=1;
    for (; i<t.size(); ++i) {
        char c=t[i];
        if (c=='.') { if (dotSeen) return false; dotSeen=true; }
        else if (!std::isdigit(static_cast<unsigned char>(c))) return false;
    }
    std::istringstream ss(t);
    ss >> out;
    return !ss.fail();
}
