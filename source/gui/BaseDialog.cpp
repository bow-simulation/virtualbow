#include "BaseDialog.hpp"

BaseDialog::BaseDialog(QWidget* parent)
    : QDialog(parent, Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint | Qt::WindowTitleHint)
{

}

// Don't close Dialog on enter
// https://stackoverflow.com/a/15845565/4692009
// Todo: Why doesn't this work: btbox->addButton(QDialogButtonBox::Ok)->setAutoDefault(false);
void BaseDialog::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
        return;

    QDialog::keyPressEvent(event);
}
