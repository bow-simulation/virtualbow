#include "CustomTextEdit.hpp"

CustomTextEdit::CustomTextEdit(QWidget* parent):
    QPlainTextEdit(parent),
    textChanged(false),
    trackChange(false)
{
    QObject::connect(this, &QPlainTextEdit::textChanged, this, [=]{
        if(trackChange) {
            textChanged = true;
        }
    });
}

void CustomTextEdit::focusInEvent(QFocusEvent* e)
{
    trackChange = true;
    textChanged = false;
    QPlainTextEdit::focusInEvent(e);
}

void CustomTextEdit::focusOutEvent(QFocusEvent *e)
{
    QPlainTextEdit::focusOutEvent(e);
    trackChange = false;

    if(textChanged)
    {
        textChanged = false;
        emit editingFinished();
    }
}
