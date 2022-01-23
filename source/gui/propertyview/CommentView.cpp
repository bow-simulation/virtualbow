#include "CommentView.hpp"
#include "gui/viewmodel/DataViewModel.hpp"
#include <QFontDatabase>
#include <QSignalBlocker>

CommentView::CommentView(DataViewModel* model)
    : model(model)
{
    this->setWordWrapMode(QTextOption::NoWrap);
    this->setPlaceholderText("No comments");

    // Update view model when the text was changed in the editor
    QObject::connect(this, &CommentView::textChanged, this, [&]{ updateModel(this); });

    // Update editor when the text has changed in the view model
    QObject::connect(model, &DataViewModel::commentsModified, this, &CommentView::updateView);

    // Set initial text from the view model
    updateView(model);
}

void CommentView::updateModel(QObject* source) {
    model->setComments(this->toPlainText(), source);
}

void CommentView::updateView(QObject* source) {
    if(source != this) {
        QSignalBlocker blocker(this);    // Block textChanged signal
        this->setPlainText(model->getComments());
    }
}
