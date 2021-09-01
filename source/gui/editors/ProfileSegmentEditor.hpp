#pragma once
#include <QtWidgets>
#include "solver/model/ProfileCurve.hpp"
#include "gui/widgets/DoubleSpinBox.hpp"

class ProfileSegmentEditor: public QTableWidget {
public:
    ProfileSegmentEditor(int rows, const QList<ConstraintType>& types, const QList<QString>& names, const QList<UnitGroup*>& units);

    SegmentInput getData() const;
    void setData(const SegmentInput& data);

private:
    QList<ConstraintType> types;
    QList<UnitGroup*> units;

    QList<QComboBox*> combo_boxes;
    QList<DoubleSpinBox*> spinners;
};
