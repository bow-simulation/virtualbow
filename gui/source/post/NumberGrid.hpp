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
    void addValues(const QString& name, const QList<double>& values, const QList<const Quantity*> quantities);
    void addValue(const QString& name, double value, const Quantity& quantity);

private:
    QHBoxLayout* columnLayout;
    QVBoxLayout* currentColumn;
    QGridLayout* currentGrid;
};
