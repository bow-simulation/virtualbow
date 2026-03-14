#pragma once
#include <QWidget>

class Quantity;
class QHBoxLayout;
class QVBoxLayout;
class QGridLayout;

class OutputGrid : public QWidget {
public:
    OutputGrid();

    void addColumn();
    void addGroup(const QString& name);
    void addHeaders(const QStringList& headers);

    void addValue(const QString& name, QWidget* widget);
    void addValues(const QString& name, QList<QWidget*> widgets);

    //void addValues(const QString& name, const QList<double>& values, const QList<const Quantity*> quantities, const QList<double>& allowed = {}, const QList<double>& maximum = {}, int decimals = 2);
    //void addValue(const QString& name, double value, const Quantity& quantity, int decimals = 2);

private:
    QHBoxLayout* columnLayout;
    QVBoxLayout* currentColumn;
    QGridLayout* currentGrid;
};
