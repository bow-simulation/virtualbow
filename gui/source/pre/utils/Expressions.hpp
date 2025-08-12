#pragma once
#include <QString>
#include <optional>

// Parses a string of numbers and mathematical operators into an expression and evaluates it.
// Returns a double if successful or nothing if the expression was invalid.
std::optional<double> evalExpression(const QString& input);

// Cheks if a string of numbers and mathematical operators is a valid expression
bool checkExpression(const QString& input);
