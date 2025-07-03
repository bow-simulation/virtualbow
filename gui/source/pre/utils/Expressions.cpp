#include "Expressions.hpp"

calculate::Parser parser = calculate::Parser{};

Expression parseExpression(const QString& input) {
    return parser.parse(input.toStdString());
}

bool checkExpression(const QString& input) {
    try {
        parseExpression(input);
        return true;
    }
    catch(calculate::BaseError&) {
        return false;
    }
}
