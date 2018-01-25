#include "BowEditor.hpp"

BowEditor::BowEditor()
    : QSplitter(Qt::Horizontal),
      edit(new TreeEditor()),
      view(new LimbView())
{
    this->addWidget(edit);
    this->setStretchFactor(0, 0);

    this->addWidget(view);
    this->setStretchFactor(1, 1);

    QObject::connect(edit, &TreeEditor::modified, [&]{
        view->setData(edit->getData());
        emit modified();
    });
}

const InputData& BowEditor::getData() const
{
    return edit->getData();
}

void BowEditor::setData(const InputData& data)
{
    edit->setData(data);
    view->setData(data);
}
