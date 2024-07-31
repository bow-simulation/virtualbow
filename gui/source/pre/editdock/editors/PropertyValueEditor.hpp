#pragma once
#include "SegmentEditor.hpp"
#include "solver/model/profile/ProfileInput.hpp"
#include "pre/widgets/DoubleSpinBox.hpp"
#include <QComboBox>
#include <QStackedWidget>

class Quantity;

class PropertyValueEditor: public SegmentEditor
{
public:
    PropertyValueEditor(int rows, const QList<QString>& names,  const QList<Quantity*>& quantities, const QList<DoubleRange>& ranges);

protected:
    template<typename KeyType>
    void setProperties(const std::map<KeyType, double>& data) {
        QSignalBlocker blocker(this);    // Block modification signals

        int row = 0;
        for(auto entry: data) {
            combos[row]->setCurrentIndex(static_cast<int>(entry.first));
            static_cast<DoubleSpinBox*>(stacks[row]->currentWidget())->setValue(entry.second);
            ++row;
        }
    }

    template<typename KeyType>
    std::map<KeyType, double> getProperties() const {
        std::map<KeyType, double> data;
        for(int row = 0; row < combos.size(); ++row) {
            KeyType key = static_cast<KeyType>(combos[row]->currentIndex());
            data[key] = static_cast<DoubleSpinBox*>(stacks[row]->currentWidget())->value();
        }

        return data;
    }

private:
    QList<QComboBox*> combos;
    QList<QStackedWidget*> stacks;
};
