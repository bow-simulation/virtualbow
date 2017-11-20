#pragma once
#include <QtWidgets>

// Validator that retains the last valid input and resets to that if the widget is left in an invalid state
class TableValidator: public QDoubleValidator
{
public:
    State validate(QString &input, int &pos) const
    {
        State state = QDoubleValidator::validate(input, pos);
        if(state == QValidator::Acceptable)
            value = QLocale::c().toDouble(input);

        return state;
    }

    void fixup(QString& input) const
    {
        input = QLocale::c().toString(value, 'g');
    }

private:
    mutable double value = 0.0;
};

class TableWidget: public QTableWidget
{
public:
    TableWidget(const QStringList& labels)
        : QTableWidget(0, labels.size())
    {
        setHorizontalHeaderLabels(labels);
        horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
        verticalHeader()->sectionResizeMode(QHeaderView::Fixed);
        verticalHeader()->setDefaultSectionSize(horizontalHeader()->height());    // Todo: Better way?
        verticalHeader()->hide();
    }

    double getValue(int row, int col) const
    {
        QLocale::c().toDouble(cellEdit(row, col)->text());
    }

    void setValue(int row, int col, double value)
    {
        if(row >= rowCount())
            addRows(row - rowCount() + 1);

        if(col >= columnCount())
            addCols(col - columnCount() + 1);

        cellEdit(row, col)->setText(QLocale::c().toString(value, 'g'));
    }

private:
    void addRows(int n)
    {
        int old_rows = rowCount();
        setRowCount(old_rows + n);

        for(int i = old_rows; i < rowCount(); ++i)
        {
            for(int j = 0; j < columnCount(); ++j)
                setCellWidget(i, j, createWidget());
        }
    }

    void addCols(int n)
    {
        int old_cols = columnCount();
        setColumnCount(old_cols + n);

        for(int i = 0; i < rowCount(); ++i)
        {
            for(int j = old_cols; j < columnCount(); ++j)
                setCellWidget(i, j, createWidget());
        }
    }

    QLineEdit* createWidget()
    {
        QLineEdit *edit = new QLineEdit();
        edit->setValidator(new TableValidator());
        return edit;
    }

    QLineEdit* cellEdit(int row, int col) const
    {
        return dynamic_cast<QLineEdit*>(cellWidget(row, col));
    }
};

