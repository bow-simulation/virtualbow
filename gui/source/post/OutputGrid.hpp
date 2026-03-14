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

private:
    QHBoxLayout* columnLayout;
    QVBoxLayout* currentColumn;
    QGridLayout* currentGrid;
};
