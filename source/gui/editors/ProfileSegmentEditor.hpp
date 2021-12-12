#pragma once
#include <QtWidgets>
#include "solver/model/profile/ProfileInput.hpp"
#include "gui/widgets/DoubleSpinBox.hpp"

class ProfileSegmentEditor: public QWidget {
    Q_OBJECT

public:
    ProfileSegmentEditor();
    virtual SegmentInput getData() const = 0;

signals:
    void modified();
};

class PropertyValueEditor: public ProfileSegmentEditor
{
public:
    PropertyValueEditor(int rows, const QList<QString>& names,  const QList<UnitGroup*>& units)
        : units(units)
    {
        auto table = new QTableWidget(rows, 2);
        table->setHorizontalHeaderLabels({"Property", "Value"});
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        table->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
        table->verticalHeader()->setVisible(false);

        for(int i = 0; i < rows; ++i) {
            auto combo = new QComboBox();
            combo->setFrame(false);
            for(int j = 0; j < names.size(); ++j) {
                combo->addItem(names[j], j);
            }

            auto spinner = new DoubleSpinBox(Domain::UNRESTRICTED);
            spinner->setFrame(false);

            // Update the unis of the spinner depending on the combobox selection
            auto update_spinner_unit = [this, spinner](int index) {
                QSignalBlocker blocker(spinner);    // Block modification signal
                spinner->setValue(0.0);
                spinner->setUnitGroup(this->units[index]);
            };

            /*
            // Set the selected entry to disabled in all other comboboxes
            auto update_combos_enabled = [this, combo](int index) {
                for(auto c: combos) {
                    // http://stackoverflow.com/q/38915001
                    auto model = qobject_cast<QStandardItemModel*>(c->model());
                    for(int i = 0; i < model->rowCount(); ++i) {
                        QStandardItem* item = model->item(i);
                        if(i == index && c != combo) {
                            item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
                        }
                        else if(c != combo) {
                            item->setFlags(item->flags() | Qt::ItemIsEnabled);
                        }
                    }
                }
            };
            */

            QObject::connect(spinner, QOverload<double>::of(&DoubleSpinBox::valueChanged), this, &ProfileSegmentEditor::modified);
            QObject::connect(combo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ProfileSegmentEditor::modified);
            QObject::connect(combo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, update_spinner_unit);
            //QObject::connect(combo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, update_combos_enabled);
            update_spinner_unit(combo->currentIndex());

            combos.push_back(combo);
            spinners.push_back(spinner);

            table->setCellWidget(i, 0, combo);
            table->setCellWidget(i, 1, spinner);
        }

        auto vbox = new QVBoxLayout();
        vbox->setMargin(0);
        vbox->addWidget(table);

        this->setLayout(vbox);
    }

protected:
    void setData(int row, int index, int value) {
        QSignalBlocker blocker(this);    // Block modification signals
        combos[row]->setCurrentIndex(index);
        spinners[row]->setValue(value);
    }

private:
    QList<QComboBox*> combos;
    QList<DoubleSpinBox*> spinners;
    QList<UnitGroup*> units;
};

class LineSegmentEditor: public PropertyValueEditor
{
public:
    LineSegmentEditor(const LineInput& input)
        : PropertyValueEditor(1, { "Length" }, { &UnitSystem::length }) {

    }

    SegmentInput getData() const override {
        return LineInput();
    }
};

class ArcSegmentEditor: public PropertyValueEditor
{
public:
    ArcSegmentEditor(const ArcInput& input)
        : PropertyValueEditor(2, { "Length", "Curvature" }, { &UnitSystem::length, &UnitSystem::curvature }) {

    }

    SegmentInput getData() const override {
        return ArcInput();
    }
};

class SpiralSegmentEditor: public PropertyValueEditor
{
public:
    SpiralSegmentEditor(const SpiralInput& input)
        : PropertyValueEditor(3, { "Length", "Curv. Start", "Curv. End" }, { &UnitSystem::length, &UnitSystem::curvature, &UnitSystem::curvature }) {

    }

    SegmentInput getData() const override {
        return SpiralInput();
    }
};

class SplineSegmentEditor: public ProfileSegmentEditor
{
public:
    SplineSegmentEditor(const SplineInput& input) {
        auto table = new QTableWidget(50, 2);
        table->setHorizontalHeaderLabels({"X [mm]", "Y [mm]"});
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        table->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
        table->verticalHeader()->setVisible(false);

        auto vbox = new QVBoxLayout();
        vbox->setMargin(0);
        vbox->addWidget(table);

        this->setLayout(vbox);
    }

    SegmentInput getData() const override {
        return SplineInput();
    }
};
