#pragma once
#include "solver/model/profile/ProfileInput.hpp"
#include "gui/widgets/DoubleSpinBox.hpp"
#include <QWidget>
#include <QComboBox>

class UnitGroup;
class DoubleSpinBox;

class SegmentEditor: public QWidget {
    Q_OBJECT

public:
    virtual SegmentInput getData() const = 0;
    virtual void setData(const SegmentInput& data) = 0;

signals:
    void modified();
};

class PropertyValueEditor: public SegmentEditor
{
public:
    PropertyValueEditor(int rows, const QList<QString>& names,  const QList<UnitGroup*>& units);

protected:
    template<typename T>
    void setProperties(const std::unordered_map<T, double>& data) {
        QSignalBlocker blocker(this);    // Block modification signals

        int row = 0;
        for(auto entry: data) {
            combos[row]->setCurrentIndex(static_cast<int>(entry.first));
            spinners[row]->setValue(entry.second);
            ++row;
        }
    }

private:
    QList<QComboBox*> combos;
    QList<DoubleSpinBox*> spinners;
    QList<UnitGroup*> units;
};

class LineSegmentEditor: public PropertyValueEditor
{
public:
    LineSegmentEditor();

    SegmentInput getData() const override;
    void setData(const SegmentInput& data);
};

class ArcSegmentEditor: public PropertyValueEditor
{
public:
    ArcSegmentEditor();

    SegmentInput getData() const override;
    void setData(const SegmentInput& data);
};

class SpiralSegmentEditor: public PropertyValueEditor
{
public:
    SpiralSegmentEditor();

    SegmentInput getData() const override;
    void setData(const SegmentInput& data);
};

class SplineSegmentEditor: public SegmentEditor
{
public:
    SplineSegmentEditor();

    SegmentInput getData() const override;
    void setData(const SegmentInput& data);
};
