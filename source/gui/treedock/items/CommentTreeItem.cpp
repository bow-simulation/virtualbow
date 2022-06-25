#include "CommentTreeItem.hpp"
#include "gui/viewmodel/ViewModel.hpp"
#include <QPlainTextEdit>

CommentTreeItem::CommentTreeItem(ViewModel* model)
    : TreeItem(model, "Comments", QIcon(":/icons/model-comments.svg"), TreeItemType::COMMENTS)
{
    auto text_edit = new QPlainTextEdit();
    text_edit->setWordWrapMode(QTextOption::NoWrap);
    text_edit->setPlaceholderText("Empty");
    setEditor(text_edit);

    updateView(nullptr);

    QObject::connect(text_edit, &QPlainTextEdit::textChanged, [=]{ updateModel(this); });
    QObject::connect(model, &ViewModel::commentModified, [=](void* source){
        updateView(source);
    });
}

void CommentTreeItem::updateModel(void* source) {
    auto text_edit = static_cast<QPlainTextEdit*>(editor);
    model->setComments(text_edit->toPlainText(), source);
}

void CommentTreeItem::updateView(void* source) {
    if(source != this) {
        auto text_edit = static_cast<QPlainTextEdit*>(editor);
        QSignalBlocker blocker(text_edit);

        text_edit->setPlainText(model->getComments());
    }
}
