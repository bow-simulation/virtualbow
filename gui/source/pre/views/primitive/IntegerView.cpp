#include "IntegerView.hpp"
#include "pre/utils/IntegerRange.hpp"
#include <QAbstractItemModel>
#include <QWheelEvent>
#include <cmath>

calculate::Parser IntegerView::parser = calculate::Parser{};

IntegerView::IntegerView(QAbstractItemModel* model, QPersistentModelIndex index, const IntegerRange& range) {
    setMinimum(range.min);
    setMaximum(range.max);

    // Prevent catching focus when scrolling, https://stackoverflow.com/a/19382766
    setFocusPolicy(Qt::StrongFocus);

    // Keep value up to date
    QObject::connect(this, &QSpinBox::valueChanged, this, [=](int value){ model->setData(index, value); });
    setValue(model->data(index, Qt::DisplayRole).toInt());
}

// Ignore scroll events if the widget doesn't have focus, https://stackoverflow.com/a/19382766
void IntegerView::wheelEvent(QWheelEvent *event) {
    if(!hasFocus()) {
        event->ignore();
    }
    else {
        QSpinBox::wheelEvent(event);
    }
}

int IntegerView::valueFromText(const QString& text) const {
    QString input = text;
    input.remove(suffix());

    auto expression = IntegerView::parser.parse(input.toStdString());
    return expression();
}

QValidator::State IntegerView::validate(QString& text, int& pos) const {
    QString input = text;
    input.remove(suffix());

    try {
        auto expression = IntegerView::parser.parse(input.toStdString());
        expression();

        return QValidator::Acceptable;
    }
    catch(calculate::BaseError&) {
        return QValidator::Intermediate;
    }
}
