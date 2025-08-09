#pragma once
#include <QString>

// Parses a string of numbers and mathematical operators into an expression that can be evaluated
double evalExpression(const QString& input);

// Cheks if a string of numbers and mathematical operators is a valid expression
bool checkExpression(const QString& input);
