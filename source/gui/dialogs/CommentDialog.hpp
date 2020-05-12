#pragma once
#include <QtWidgets>
#include "gui/PersistentDialog.hpp"
#include <string>

class CommentDialog: public PersistentDialog
{
    Q_OBJECT

public:
    CommentDialog(QWidget* parent);

    std::string getData() const;
    void setData(const std::string& text);

signals:
    void modified();

private:
    QTextEdit* edit;
    bool changed;
};
