#include "CommentTreeItem.hpp"
#include "gui/viewmodel/DataViewModel.hpp"
#include <QPlainTextEdit>

CommentTreeItem::CommentTreeItem(DataViewModel* model)
    : TreeItem("Comments", QIcon(":/icons/model-comments.svg"), TreeItemType::COMMENTS),
      model(model)
{

}

QWidget* CommentTreeItem::createEditor() const {
    // Create editor
    auto editor = new QPlainTextEdit();
    editor->setWordWrapMode(QTextOption::NoWrap);
    editor->setPlaceholderText("Empty");

    // Set initial text from the view model
    editor->setPlainText(QString::fromStdString(model->getData().comment));

    // Update view model when text changed in the editor
    QObject::connect(editor, &QPlainTextEdit::textChanged, [&, editor] {
        model->getData().comment = editor->toPlainText().toStdString();
        model->commentModified(editor);
    });

    return editor;
}
