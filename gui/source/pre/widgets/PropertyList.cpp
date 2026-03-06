#include "PropertyList.hpp"
#include <QGridLayout>
#include <QVBoxLayout>
#include <QLabel>

const int EDITOR_WIDTH = 130;

PropertyList::PropertyList(QWidget* parent):
    QWidget(parent)
{
    grid = new QGridLayout();
    grid->setHorizontalSpacing(10);
    grid->setColumnStretch(0, 1);
    grid->setColumnStretch(1, 0);

    setLayout(grid);
}

int PropertyList::addHeading(const QString& title) {
    auto label = new QLabel(title + ":");
    label->setStyleSheet("font-weight: bold;");
    label->setMinimumHeight(30);

    int rowCount = grid->rowCount();
    grid->addWidget(label, rowCount, 0);

    return rowCount;
}

int PropertyList::addProperty(const QString& name, QWidget* editor) {
    auto label = new QLabel(name);
    editor->setFixedWidth(EDITOR_WIDTH);

    int rowCount = grid->rowCount();
    grid->addWidget(label, rowCount, 0, Qt::AlignTop);
    grid->addWidget(editor, rowCount, 1, Qt::AlignTop);

    return rowCount;
}

int PropertyList::addWidget(QWidget* widget) {
    int rowCount = grid->rowCount();
    grid->addWidget(widget, rowCount, 0, 1, 2);

    return rowCount;
}

int PropertyList::addStretch() {
    int rowCount = grid->rowCount();
    grid->setRowStretch(rowCount, 1);

    return rowCount;
}

void PropertyList::setVisible(int row, bool value) {
    // Iterate over items in grid
    for (int i = 0; i < grid->count(); ++i) {
        QLayoutItem* item = grid->itemAt(i);

        int r, c, rs, cs;
        grid->getItemPosition(i, &r, &c, &rs, &cs);

        // Check if the item occupies the row
        if (r <= row && row < r + rs) {
            if(QWidget* widget = item->widget()) {
                widget->setVisible(value);
            }
        }
    }
}
