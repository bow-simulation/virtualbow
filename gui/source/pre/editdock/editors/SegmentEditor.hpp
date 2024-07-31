#pragma once
#include "solver/model/profile/ProfileInput.hpp"
#include "pre/widgets/DoubleSpinBox.hpp"
#include <QWidget>
#include <QComboBox>

class Quantity;
class DoubleSpinBox;

class SegmentEditor: public QWidget {
    Q_OBJECT

public:
    virtual SegmentInput getData() const = 0;
    virtual void setData(const SegmentInput& data) = 0;

signals:
    void modified();
};
