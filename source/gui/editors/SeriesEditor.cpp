#include "SeriesEditor.hpp"

void TableItem::setData(int role, const QVariant& data)
{  
    bool valid;
    QLocale().toDouble(data.toString(), &valid);

    if(!valid)
        qInfo() << "Not valid: " << data;

    if(valid || data.toString().isEmpty())
        QTableWidgetItem::setData(role, data);
}

SeriesEditor::SeriesEditor(const QString& x_label, const QString& y_label, int rows)
    : QTableWidget(rows, 2)
{
    this->setSelectionMode(QAbstractItemView::ContiguousSelection);
    this->setHorizontalHeaderLabels({x_label, y_label});
    this->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    this->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    this->verticalHeader()->sectionResizeMode(QHeaderView::Fixed);
    this->verticalHeader()->setDefaultSectionSize(horizontalHeader()->height());    // Todo: Better way?
    this->verticalHeader()->hide();

    for(int i = 0; i < rowCount(); ++i)
    {
        for(int j = 0; j < columnCount(); ++j)
            this->setItem(i, j, new TableItem());
    }

    // Actions

    auto action_cut = new QAction("&Cut", this);
    QObject::connect(action_cut, &QAction::triggered, this, &SeriesEditor::cutSelection);
    action_cut->setShortcuts(QKeySequence::Cut);
    this->addAction(action_cut);

    auto action_copy = new QAction("Cop&y", this);
    QObject::connect(action_copy, &QAction::triggered, this, &SeriesEditor::copySelection);
    action_copy->setShortcuts(QKeySequence::Copy);
    this->addAction(action_copy);

    auto action_paste = new QAction("&Paste", this);
    QObject::connect(action_paste, &QAction::triggered, this, &SeriesEditor::pasteToSelection);
    action_paste->setShortcuts(QKeySequence::Paste);
    this->addAction(action_paste);

    auto action_delete = new QAction("&Delete", this);
    QObject::connect(action_delete, &QAction::triggered, this, &SeriesEditor::deleteSelection);
    action_delete->setShortcuts(QKeySequence::Delete);
    this->addAction(action_delete);

    // Context menu

    this->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(this, &SeriesEditor::customContextMenuRequested, [=](const QPoint& pos){
        QMenu menu(this);
        menu.addAction(action_cut);
        menu.addAction(action_copy);
        menu.addAction(action_paste);
        menu.addSeparator();
        menu.addAction(action_delete);
        menu.exec(this->viewport()->mapToGlobal(pos));
    });

    // Event handling

    QObject::connect(this, &SeriesEditor::cellChanged, [&]{
        emit this->selectionChanged(getSelectedIndices());
        if(this->state() == QAbstractItemView::EditingState) {
            emit modified();
        }
    });

    QObject::connect(this, &QTableWidget::itemSelectionChanged, this, [&] {
        emit this->selectionChanged(getSelectedIndices());
    });
}

Series SeriesEditor::getData() const
{
    Series data;
    for(int i = 0; i < rowCount(); ++i)
    {
        bool arg_valid, val_valid;
        double arg = QLocale().toDouble(this->item(i, 0)->text(), &arg_valid);
        double val = QLocale().toDouble(this->item(i, 1)->text(), &val_valid);

        if(arg_valid && val_valid)
            data.push_back(arg, val);
    }

    return data;
}

void SeriesEditor::setData(const Series& data)
{
    for(int i = 0; i < rowCount(); ++i)
    {
        if(i < data.size())
        {
            this->item(i, 0)->setText(QLocale().toString(data.arg(i), 'g'));
            this->item(i, 1)->setText(QLocale().toString(data.val(i), 'g'));
        }
        else
        {
            this->item(i, 0)->setText("");
            this->item(i, 1)->setText("");
        }
    }
}

void SeriesEditor::cutSelection()
{
    copySelection();
    deleteSelection();
}

void SeriesEditor::copySelection()
{
    auto selection = this->selectedRanges();
    if(selection.size() != 1)
        return;

    QString content;
    for(int i = selection[0].topRow(); i <= selection[0].bottomRow(); ++i)
    {
        for(int j = selection[0].leftColumn(); j <= selection[0].rightColumn(); ++j)
        {
            content += this->item(i, j)->text();

            if(j != selection[0].rightColumn())
                content += "\t";
        }

        content += "\r\n";
    }

    QGuiApplication::clipboard()->setText(content);
}

void SeriesEditor::pasteToSelection()
{
    auto selection = this->selectedRanges();
    if(selection.size() != 1)
        return;

    QString content = QGuiApplication::clipboard()->text();
    QTextStream stream(&content);

    for(int i = selection[0].topRow(); i < rowCount() && !stream.atEnd(); ++i)
    {
        QStringList values = stream.readLine().split("\t");
        QStringListIterator iterator(values);

        for(int j = selection[0].leftColumn(); j < columnCount() && iterator.hasNext(); ++j)
            this->item(i, j)->setText(iterator.next());
    }

    emit modified();
}

void SeriesEditor::deleteSelection()
{
    auto selection = this->selectedRanges();
    for(auto& range: selection)
    {
        for(int i = range.topRow(); i <= range.bottomRow(); ++i)
        {
            for(int j = range.leftColumn(); j <= range.rightColumn(); ++j)
                this->item(i, j)->setText("");
        }
    }

    emit modified();
}

QVector<int> SeriesEditor::getSelectedIndices()
{
    QVector<int> selection;
    int index = 0;
    for(int i = 0; i < this->rowCount(); ++i)
    {
        bool arg_valid, val_valid;
        QLocale().toDouble(this->item(i, 0)->text(), &arg_valid);
        QLocale().toDouble(this->item(i, 1)->text(), &val_valid);

        if(arg_valid && val_valid)
        {
            if(this->item(i, 0)->isSelected() || this->item(i, 1)->isSelected())
                selection.push_back(index);

            ++index;
        }
    }

    return selection;
}
