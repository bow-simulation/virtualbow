#pragma once
#include <QWidget>

class Quantity;
class QHBoxLayout;
class QVBoxLayout;
class QGridLayout;

class NumberGrid : public QWidget {
public:
    NumberGrid();

    void addColumn();
    void addGroup(const QString& name);
    void addHeaders(const QStringList& headers);
    void addValues(const QString& name, const Quantity& quantity, const QList<double>& values);
    void addValue(const QString& name, const Quantity& quantity, double value);

private:
    QHBoxLayout* columnLayout;
    QVBoxLayout* currentColumn;
    QGridLayout* currentGrid;
};
