#include "Expressions.hpp"
#include "extern/calculate/include/calculate.hpp"


calculate::Parser parser = calculate::Parser{};

std::optional<double> evalExpression(const QString& input) {
    try {
        auto expression = parser.parse(input.toStdString());
        return expression();
    }
    catch(const calculate::BaseError&) {
        return std::nullopt;
    }
}

bool checkExpression(const QString& input) {
    return evalExpression(input).has_value();
}
