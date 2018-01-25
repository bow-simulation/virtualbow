#pragma once
#include "gui/PersistentDialog.hpp"
#include "gui/input/editors/LayerEditor.hpp"
#include "bow/input/InputData.hpp"

class WidthDialog: public PersistentDialog
{
    Q_OBJECT

public:
    WidthDialog(QWidget* parent)
        : PersistentDialog(parent, "WidthDialog", {800, 400}),    // Magic numbers
          edit(new SeriesEditor("Position", "Width [m]", 25)),
          view(new SplineView("Position", "Width [m]"))
    {
        auto buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        QObject::connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
        QObject::connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

        auto hbox = new QHBoxLayout();
        hbox->addWidget(edit, 0);
        hbox->addWidget(view, 1);

        auto vbox = new QVBoxLayout();
        vbox->addLayout(hbox, 1);
        vbox->addWidget(buttons, 0);

        this->setWindowTitle("Width");
        this->setLayout(vbox);

        // Event handling

        QObject::connect(edit, &SeriesEditor::modified, [&]{
            view->setData(edit->getData());
            emit modified();
        });
    }

    Series getData() const
    {
        return edit->getData();
    }

    void setData(const Series& width)
    {
        edit->setData(width);
        view->setData(width);
    }

signals:
    void modified();

private:
    SeriesEditor* edit;
    SplineView* view;
};
