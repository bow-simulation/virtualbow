#include "Expressions.hpp"
#include "extern/calculate/include/calculate.hpp"

calculate::Parser parser = calculate::Parser{};

double evalExpression(const QString& input) {
    auto expression = parser.parse(input.toStdString());
    return expression();
}

bool checkExpression(const QString& input) {
    try {
        evalExpression(input);
        return true;
    }
    catch(calculate::BaseError&) {
        return false;
    }
}
