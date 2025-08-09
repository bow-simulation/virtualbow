#include "ErrorLabel.hpp"

const QString ERROR_LABEL_STYLE = "font-weight: bold; padding: 10px; background-color: rgb(253, 200, 53); border-color: rgb(230, 0, 0); border-radius: 8px; border-style: solid; border-width: 2px";

ErrorLabel::ErrorLabel(QWidget* parent):
    QLabel(parent)
{
    setStyleSheet(ERROR_LABEL_STYLE);
    setWordWrap(true);
    setAlignment(Qt::AlignCenter);
}

QSize ErrorLabel::sizeHint() const {
    return QSize(600, QWidget::sizeHint().height());
}
