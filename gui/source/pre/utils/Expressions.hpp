#pragma once
#include "extern/calculate/include/calculate.hpp"
#include <QString>

using Expression = calculate::Node<calculate::BaseParser<double>>;

// Parses a string of numbers and mathematical operators into an expression that can be evaluated
Expression parseExpression(const QString& input);

// Cheks if a string of numbers and mathematical operators is a valid expression
bool checkExpression(const QString& input);
