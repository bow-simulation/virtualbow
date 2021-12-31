#pragma once
#include "solver/model/profile/ProfileInput.hpp"
#include "gui/modeltree/profile/SegmentEditor.hpp"
#include "gui/widgets/DoubleSpinBox.hpp"
#include <QComboBox>

class UnitGroup;
class DoubleSpinBox;

class PropertyValueEditor: public SegmentEditor
{
public:
    PropertyValueEditor(int rows, const QList<QString>& names,  const QList<UnitGroup*>& units);

protected:
    template<typename KeyType>
    void setProperties(const std::unordered_map<KeyType, double>& data) {
        QSignalBlocker blocker(this);    // Block modification signals

        int row = 0;
        for(auto entry: data) {
            combos[row]->setCurrentIndex(static_cast<int>(entry.first));
            spinners[row]->setValue(entry.second);
            ++row;
        }
    }

    template<typename KeyType>
    std::unordered_map<KeyType, double> getProperties() const {
        std::unordered_map<KeyType, double> data;
        for(int row = 0; row < combos.size(); ++row) {
            KeyType key = static_cast<KeyType>(combos[row]->currentIndex());
            data[key] = spinners[row]->value();
        }

        return data;
    }

private:
    QList<QComboBox*> combos;
    QList<DoubleSpinBox*> spinners;
    QList<UnitGroup*> units;
};
