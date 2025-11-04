#ifndef BITCOINEXCHANGE_HPP
#define BITCOINEXCHANGE_HPP

#include <map>
#include <string>

class BitcoinExchange {
public:
    BitcoinExchange();
    void loadRates(const std::string& csvPath); // lee data.csv
    bool hasRates() const;

    // Devuelve true y asigna 'rate' de la fecha exacta o de la más cercana inferior.
    // Devuelve false si no hay fecha <= date en la BD.
    bool getRateForDate(const std::string& date, double& rate) const;

    // Validaciones utilitarias
    static bool isValidDate(const std::string& ymd);
    static bool parseNumber(const std::string& s, double& out);

private:
    std::map<std::string, double> _rates; // YYYY-MM-DD -> rate
};

#endif
