#pragma once
#include <QtWidgets>
#include "solver/numerics/Eigen.hpp"
#include "gui/units/UnitGroup.hpp"
#include "TableView.hpp"
#include "TableModel.hpp"

class ProfileTreeHeader: public QHeaderView {
public:
    ProfileTreeHeader(QWidget* parent, const QList<QToolButton*>& buttons);
};

class ProfileEditor: public QWidget
{
    Q_OBJECT

public:
    ProfileEditor(const UnitSystem& units);

    MatrixXd getData() const;
    void setData(const MatrixXd& data);

signals:
    void modified();
    void rowSelectionChanged(const QVector<int>& rows);

private:    
    QVector<int> getSelectedIndices();
};
