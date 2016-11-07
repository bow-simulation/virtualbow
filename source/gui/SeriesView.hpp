#pragma once
#include "../model/Document.hpp"
#include "../numerics/Series.hpp"
#include <QtWidgets>

class SeriesView: public QWidget
{
public:
    SeriesView(const QString& lb_args, const QString& lb_vals, DocItem<Series>& doc_item)
        : doc_item(doc_item)
    {
        // Widgets and Layout

        auto vbox = new QVBoxLayout();
        this->setLayout(vbox);

        table = new QTableWidget(1, 2);
        table->setHorizontalHeaderLabels({{lb_args, lb_vals}});
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        table->verticalHeader()->sectionResizeMode(QHeaderView::Fixed);
        table->verticalHeader()->setDefaultSectionSize(table->horizontalHeader()->height());    // Todo: Better way?
        table->verticalHeader()->hide();
        vbox->addWidget(table);

        //table->setSelectionBehavior(QAbstractItemView::SelectRows);

        auto bt_insert_below = new QPushButton(QIcon(":/icons/insert-below"), "");
        auto bt_insert_above = new QPushButton(QIcon(":/icons/insert-above"), "");
        auto bt_delete = new QPushButton(QIcon(":/icons/edit-delete"), "");
        bt_insert_below->setToolTip("Insert below");
        bt_insert_above->setToolTip("Insert above");
        bt_delete->setToolTip("Delete selected rows");

        auto hbox = new QHBoxLayout();
        hbox->addWidget(bt_insert_below);
        hbox->addWidget(bt_insert_above);
        hbox->addWidget(bt_delete);
        vbox->addLayout(hbox);

        // Event handling

        connection = doc_item.connect([this](const Series& series)
        {
            setData(series);
        });

        QObject::connect(table, &QTableWidget::cellChanged, [this](int i, int j)
        {
            if(!table->item(i, j)->isSelected())    // Make sure the cell was changed by the user
                return;

            try
            {
                double value = getCellValue(i, j);
                Series series = this->doc_item;

                if(j == 0)
                    series.arg(i) = value;
                else if(j == 1)
                    series.val(i) = value;

                this->doc_item = series;
            }
            catch(const std::runtime_error&)
            {
                const Series& series = this->doc_item;

                if(j == 0)
                    setCellValue(i, j, series.arg(i));
                else if(j == 1)
                    setCellValue(i, j, series.val(i));
            }
        });

        QObject::connect(bt_insert_below, &QPushButton::clicked, [this]()
        {
             insertRow(false);
        });

        QObject::connect(bt_insert_above, &QPushButton::clicked, [this]()
        {
            insertRow(true);
        });

        QObject::connect(bt_delete, &QPushButton::clicked, [this]()
        {
            if(!deleteSelectedRows())
            {
                deleteLastRow();
            }
        });
    }

private:
    DocItem<Series>& doc_item;
    Connection connection;
    QTableWidget* table;

    void setData(const Series& series)
    {
        table->setRowCount(series.size());

        for(size_t i = 0; i < series.size(); ++i)
        {
            setCellValue(i, 0, series.arg(i));
            setCellValue(i, 1, series.val(i));
        }
    }

    void setCellValue(int i, int j, double value)
    {
        if(table->item(i, j) == nullptr)
            table->setItem(i, j,  new QTableWidgetItem);

        table->item(i, j)->setText(QLocale::c().toString(value, 'g', 15));    // Todo: Magic number
    }

    double getCellValue(int i, int j) const
    {
        bool ok;
        double value = QLocale::c().toDouble(table->item(i, j)->text(), &ok);

        if(!ok)
        {
            throw std::runtime_error("Cannot convert input to number");
        }

        return value;
    }

    void insertRow(bool above)
    {
        auto selection = table->selectedRanges();
        Series series = this->doc_item;

        size_t index;
        switch(selection.size())
        {
        case 0:
            // No range selected: Insert at top or bottom of the table
            index = above ? 0 : series.size();
            break;

        case 1:
            // One range selected: Insert above or below the selection
            if(above)
            {
                index = selection[0].topRow();

                // Move selection
                QTableWidgetSelectionRange new_range(selection[0].topRow() + 1, selection[0].leftColumn(),
                                                     selection[0].bottomRow() + 1, selection[0].rightColumn());
                table->setRangeSelected(selection[0], false);
                table->setRangeSelected(new_range, true);
            }
            else
            {
                index = selection[0].bottomRow() + 1;
            }

            break;

        default:
            // Multiple ranges selected: Do nothing.
            return;
        }

        series.insert(index, 0.0, 0.0);  // Todo: Magic numbers
        this->doc_item = series;
    }

    void deleteLastRow()
    {
        Series series = this->doc_item;
        series.remove();
        this->doc_item = series;
    }

    bool deleteSelectedRows()
    {
        auto selection = table->selectedRanges();
        if(selection.isEmpty())
            return false;

        Series series = this->doc_item;
        for(int i = selection.size()-1; i >= 0; --i)
        {
            table->setRangeSelected(selection[i], false);
            for(int j = selection[i].bottomRow(); j >= selection[i].topRow(); --j)
            {
                series.remove(j);
            }
        }

        this->doc_item = series;
        return true;
    }

    virtual void keyPressEvent(QKeyEvent* event) override
    {
        if(event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace)
        {
            deleteSelectedRows();
        }
    }
};
