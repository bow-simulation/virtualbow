#include "CommentTreeItem.hpp"
#include "gui/viewmodel/DataViewModel.hpp"
#include <QPlainTextEdit>

CommentTreeItem::CommentTreeItem(DataViewModel* model)
    : TreeItem("Comments", QIcon(":/icons/model-comments.svg"), TreeItemType::COMMENTS),
      model(model)
{
    // Create editor
    auto text_edit = new QPlainTextEdit();
    text_edit->setWordWrapMode(QTextOption::NoWrap);
    text_edit->setPlaceholderText("Empty");
    setEditor(text_edit);

    updateView();
    QObject::connect(text_edit, &QPlainTextEdit::textChanged, [&] { updateModel(); });
}

void CommentTreeItem::updateModel() {
    model->setComments(static_cast<QPlainTextEdit*>(editor)->toPlainText());
}

void CommentTreeItem::updateView() {
    static_cast<QPlainTextEdit*>(editor)->setPlainText(model->getComments());
}
