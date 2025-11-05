#ifndef RPN_HPP
#define RPN_HPP

#include <stack>
#include <string>

class RPN {
public:
    static bool evaluate(const std::string& expr, double& result);
};

#endif
