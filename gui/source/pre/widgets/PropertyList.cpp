#include "PropertyList.hpp"
#include <QGridLayout>
#include <QVBoxLayout>
#include <QLabel>

const int EDITOR_WIDTH = 120;

PropertyList::PropertyList(QWidget* parent):
    QWidget(parent)
{
    grid = new QGridLayout();
    grid->setHorizontalSpacing(10);
    grid->setColumnStretch(0, 1);
    grid->setColumnStretch(1, 0);

    setLayout(grid);
}

void PropertyList::addHeading(const QString& title) {
    auto label = new QLabel(title + ":");
    label->setStyleSheet("font-weight: bold;");
    label->setMinimumHeight(30);

    int rowCount = grid->rowCount();
    grid->addWidget(label, rowCount, 0);
}

void PropertyList::addProperty(const QString& name, const QString& tooltip, QWidget* editor) {
    auto label = new QLabel(name);
    label->setToolTip(tooltip);

    editor->setMinimumWidth(EDITOR_WIDTH);
    editor->setMaximumWidth(EDITOR_WIDTH);

    int rowCount = grid->rowCount();
    grid->addWidget(label, rowCount, 0);
    grid->addWidget(editor, rowCount, 1);
}

void PropertyList::addWidget(const QString& tooltip, QWidget* widget) {
    widget->setToolTip(tooltip);

    int rowCount = grid->rowCount();
    grid->addWidget(widget, rowCount, 0, 1, 2);
}

void PropertyList::addStretch() {
    int rowCount = grid->rowCount();
    grid->setRowStretch(rowCount, 1);
}
