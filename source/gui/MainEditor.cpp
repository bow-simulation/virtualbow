#include "MainEditor.hpp"
#include "modeltree/ModelTreeEditor.hpp"
#include "limbview/LimbView.hpp"

MainEditor::MainEditor()
    : QSplitter(Qt::Horizontal),
      edit(new ModelTreeEditor()),
      view(new LimbView())
{
    this->addWidget(edit);
    this->setStretchFactor(0, 0);
    this->setChildrenCollapsible(false);

    this->addWidget(view);
    this->setStretchFactor(1, 1);

    QObject::connect(edit, &ModelTreeEditor::modified, [&] {
        view->setData(edit->getData());
        emit modified();
    });
}

InputData MainEditor::getData() const {
    return edit->getData();
}

void MainEditor::setData(const InputData& data) {
    edit->setData(data);
    view->setData(data);
    view->view3D();    // TODO: Move this somewhere else
}
