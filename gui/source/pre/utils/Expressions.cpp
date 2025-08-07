#include "Expressions.hpp"
#include "extern/calculate/include/calculate.hpp"

calculate::Parser parser = calculate::Parser{};

double evalExpression(const QString& input) {
    try {
        auto expression = parser.parse(input.toStdString());
        return expression();
    }
    catch(const calculate::BaseError&) {
        throw std::runtime_error("Invalid expression");
    }
}

bool checkExpression(const QString& input) {
    try {
        evalExpression(input);
        return true;
    }
    catch(const calculate::BaseError&) {
        return false;
    }
}
