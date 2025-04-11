#include "TextView.hpp"
#include <QAbstractItemModel>

TextView::TextView(QAbstractItemModel* model, QPersistentModelIndex index) {
    setWordWrapMode(QTextOption::NoWrap);
    setPlaceholderText("Empty");

    QObject::connect(this, &QPlainTextEdit::textChanged, this, [=]{
        model->setData(index, toPlainText());
    });

    setPlainText(model->data(index, Qt::DisplayRole).toString());
}
