#pragma once
#include "solver/numerics/EigenTypes.hpp"
#include "gui/viewmodel/units/UnitGroup.hpp"
#include "TableView.hpp"
#include "TableModel.hpp"
#include "TableSpinner.hpp"

class TableEditor: public TableView {
    Q_OBJECT

public:
    TableEditor(const QString& x_label, const QString& y_label, const TableSpinnerOptions& x_options, const TableSpinnerOptions& y_options);

    std::vector<Vector<2>> getData() const;
    void setData(const std::vector<Vector<2>>& data);

signals:
    void modified();
    void rowSelectionChanged(const QVector<int>& rows);

private:
    TableModel model;
    QVector<int> getSelectedIndices();
};
