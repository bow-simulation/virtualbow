#pragma once
#include "solver/numerics/Eigen.hpp"
#include <QtWidgets>

class TableItem: public QTableWidgetItem
{
    virtual void setData(int role, const QVariant& data);
};

class SeriesEditor: public QTableWidget
{
    Q_OBJECT

public:
    SeriesEditor(const QString& x_label, const QString& y_label, int rows);
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

    QVector<int> getSelectedRows();
};

