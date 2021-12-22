#pragma once
#include "solver/numerics/EigenTypes.hpp"
#include "gui/units/UnitGroup.hpp"
#include "TableView.hpp"
#include "TableModel.hpp"

class TableEditor: public TableView {
    Q_OBJECT

public:
    TableEditor(const QString& x_label, const QString& y_label, const UnitGroup& x_unit, const UnitGroup& y_unit);

    std::vector<Vector<2>> getData() const;
    void setData(const std::vector<Vector<2>>& data);

signals:
    void modified();
    void rowSelectionChanged(const QVector<int>& rows);

private:
    TableModel model;
    QVector<int> getSelectedIndices();
};
