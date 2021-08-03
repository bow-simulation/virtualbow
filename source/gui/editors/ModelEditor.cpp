#include "ModelEditor.hpp"

ModelEditor::ModelEditor(const UnitSystem& units)
    : QSplitter(Qt::Horizontal),
      edit(new TreeEditor(units)),
      view(new LimbView())
{
    this->addWidget(edit);
    this->setStretchFactor(0, 0);
    this->setChildrenCollapsible(false);

    this->addWidget(view);
    this->setStretchFactor(1, 1);

    QObject::connect(edit, &TreeEditor::modified, [&]{
        view->setData(edit->getData());
        emit modified();
    });
}

InputData ModelEditor::getData() const
{
    return edit->getData();
}

void ModelEditor::setData(const InputData& data)
{
    edit->setData(data);
    view->setData(data);
    view->view3D();    // TODO: Move this somewhere else
}
