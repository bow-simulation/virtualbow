#include "BowEditor.hpp"
#include "gui/input/editors/TreeEditor.hpp"
#include "gui/input/views/LimbView.hpp"

BowEditor::BowEditor()
    : QSplitter(Qt::Horizontal)
{
    this->addWidget(new TreeEditor());
    this->addWidget(new LimbView());
    this->setStretchFactor(0, 0);
    this->setStretchFactor(1, 1);
}
