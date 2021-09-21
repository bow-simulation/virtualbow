#pragma once
#include <QtWidgets>
#include "solver/model/ProfileCurve.hpp"
#include "gui/widgets/DoubleSpinBox.hpp"

class ProfileSegmentEditor: public QTableWidget {
    Q_OBJECT

public:
    ProfileSegmentEditor(SegmentType segment_type);

    SegmentInput getData() const;
    void setData(const SegmentInput& data);

private:
    SegmentType segment_type;
    QList<ConstraintType> constraint_types;
    QList<QString> constraint_names;
    QList<UnitGroup*> constraint_units;

    static int numConstraints(SegmentType segment_type);
    static QList<ConstraintType> constraintTypes(SegmentType segment_type);
    static QList<QString> constraintNames(SegmentType segment_type);
    static QList<UnitGroup*> constraintUnits(SegmentType segment_type);

signals:
    void modified();

};
