#pragma once
#include "solver/numerics/EigenTypes.hpp"
#include "gui/units/UnitGroup.hpp"
#include "TableView.hpp"
#include "TableModel.hpp"

class TableEditor: public TableView {
    Q_OBJECT

public:
    TableEditor(const QList<QString>& labels, const QList<const UnitGroup*>& units);

    MatrixXd getData() const;
    void setData(const MatrixXd& data);

signals:
    void modified();
    void rowSelectionChanged(const QVector<int>& rows);

private:
    TableModel model;
    QVector<int> getSelectedIndices();
};
