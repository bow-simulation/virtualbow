#include "SeriesView.hpp"
#include "numerics/Series.hpp"

SeriesView::SeriesView(const QString& lb_args, const QString& lb_vals, DocumentItem<Series>& doc_item)
    : doc_item(doc_item),
      table(new TableWidget({lb_args, lb_vals}))
{
    // Widgets and Layout

    auto bt_insert_below = new QPushButton(QIcon(":/icons/series-view/insert-below"), "");
    auto bt_insert_above = new QPushButton(QIcon(":/icons/series-view/insert-above"), "");
    auto bt_delete = new QPushButton(QIcon(":/icons/series-view/edit-delete"), "");
    bt_insert_below->setToolTip("Insert below");
    bt_insert_above->setToolTip("Insert above");
    bt_delete->setToolTip("Delete");

    auto hbox = new QHBoxLayout();
    hbox->addWidget(bt_insert_below);
    hbox->addWidget(bt_insert_above);
    hbox->addWidget(bt_delete);

    auto vbox = new QVBoxLayout();
    this->setLayout(vbox);
    vbox->setMargin(0);
    vbox->addWidget(table);
    vbox->addLayout(hbox);

    // Event handling

    QObject::connect(&doc_item, &DocumentNode::value_changed, this, &SeriesView::update_value);
    QObject::connect(&doc_item, &DocumentNode::error_changed, this, &SeriesView::update_error);

    QObject::connect(table, &QTableWidget::cellChanged, [this](int i, int j)
    {
        if(!table->item(i, j)->isSelected())    // Make sure the cell was changed by the user
            return;

        double value = table->getValue(i, j);
        Series series = this->doc_item;

        if(j == 0)
            series.arg(i) = value;
        else if(j == 1)
            series.val(i) = value;

        this->doc_item = series;
    });

    QObject::connect(bt_insert_below, &QPushButton::clicked, [this]
    {
        insertRow(false);
    });

    QObject::connect(bt_insert_above, &QPushButton::clicked, [this]
    {
        insertRow(true);
    });

    QObject::connect(bt_delete, &QPushButton::clicked, [this]
    {
        if(!deleteSelectedRows())
        {
            deleteLastRow();
        }
    });

    QObject::connect(table, &QTableWidget::itemSelectionChanged, [&]
    {
        std::vector<int> rows;
        for(int i = 0; i < table->rowCount(); ++i)
        {
            if(table->item(i, 0)->isSelected() || table->item(i, 1)->isSelected())
                rows.push_back(i);
        }

        emit this->selectionChanged(rows);
    });

    update_value();
    update_error();
}

void SeriesView::keyPressEvent(QKeyEvent* event)
{
    if(event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace)
    {
        deleteSelectedRows();
    }
}

void SeriesView::update_value()
{
    const Series& series = doc_item;
    table->setRowCount(series.size());

    for(size_t i = 0; i < series.size(); ++i)
    {
        table->setValue(i, 0, series.arg(i));
        table->setValue(i, 1, series.val(i));
    }
}

void SeriesView::update_error()
{
    QPalette palette;
    if(doc_item.get_errors().size() == 0)
    {
        palette.setColor(QPalette::Base, Qt::white);
        this->setToolTip("");
    }
    else
    {
        palette.setColor(QPalette::Base, QColor(255, 102, 102));    // Magic number
        this->setToolTip(QString::fromStdString(doc_item.get_errors().front()));
    }
}

void SeriesView::insertRow(bool above)
{
    auto selection = table->selectedRanges();
    Series series = doc_item;

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
    doc_item = series;
}

void SeriesView::deleteLastRow()
{
    Series series = doc_item;
    series.remove();
    doc_item = series;
}

bool SeriesView::deleteSelectedRows()
{
    auto selection = table->selectedRanges();
    if(selection.isEmpty())
        return false;

    Series series = doc_item;
    for(int i = selection.size()-1; i >= 0; --i)
    {
        for(int j = selection[i].bottomRow(); j >= selection[i].topRow(); --j)
        {
            series.remove(j);
        }
    }

    doc_item = series;
    return true;
}
