#pragma once
#include "../model/InputData.hpp"
#include <QtWidgets>

class NotesDialog: public QDialog
{
public:
    NotesDialog(QWidget* parent, InputData& data)
        : QDialog(parent),
          data(data)
    {
        auto vbox = new QVBoxLayout();
        this->setLayout(vbox);
        this->setWindowTitle("Notes");

        edit = new QTextEdit();
        edit->setText(QString::fromStdString(data.meta_notes));
        vbox->addWidget(edit);

        auto btbox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        vbox->addWidget(btbox);

        QObject::connect(btbox, &QDialogButtonBox::accepted, [this]()
        {
            this->data.meta_notes = edit->toPlainText().toStdString();
            this->accept();
        });
    }

private:
    InputData& data;
    QTextEdit* edit;
};
