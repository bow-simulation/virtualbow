#pragma once
#include "solver/Input.hpp"
#include "pre/widgets/DoubleSpinBox.hpp"
#include <QWidget>
#include <QComboBox>

class Quantity;
class DoubleSpinBox;

class SegmentEditor: public QWidget {
    Q_OBJECT

public:
    virtual ProfileSegment getData() const = 0;
    virtual void setData(const ProfileSegment& data) = 0;

signals:
    void modified();
};
