#pragma once
#include "solver/numerics/EigenTypes.hpp"
#include "gui/utils/DoubleRange.hpp"
#include "TableView.hpp"
#include "TableModel.hpp"

class TableEditor: public TableView {
    Q_OBJECT

public:
    TableEditor(const QString& x_label, const QString& y_label,
                const UnitGroup& x_units, const UnitGroup& y_units,
                const DoubleRange& x_range, const DoubleRange& y_range);

    std::vector<Vector<2>> getData() const;
    void setData(const std::vector<Vector<2>>& data);

signals:
    void modified();
    void rowSelectionChanged(const QVector<int>& rows);

private:
    TableModel model;
    QVector<int> getSelectedIndices();
};
