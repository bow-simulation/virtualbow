#pragma once
#include "solver/numerics/Eigen.hpp"
#include <QtWidgets>

class TableItem: public QTableWidgetItem
{
    virtual void setData(int role, const QVariant& data);
};

class TableEditor: public QTableWidget
{
    Q_OBJECT

public:
    TableEditor(const QList<QString>& labels, int rows);
    MatrixXd getData() const;
    void setData(const MatrixXd& data);

signals:
    void modified();
    void rowSelectionChanged(const QVector<int>& rows);

private:
    void cutSelection();
    void copySelection();
    void pasteToSelection();
    void deleteSelection();

    QVector<int> getSelectedIndices();
};

