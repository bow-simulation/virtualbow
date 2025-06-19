#include "CommentsView.hpp"
#include "primitive/TextView.hpp"
#include "pre/models/CommentsModel.hpp"
#include <QVBoxLayout>

CommentsView::CommentsView(CommentsModel* model) {
    auto vbox = new QVBoxLayout(this);
    vbox->setContentsMargins(0, 0, 0, 0);

    auto view = new TextView(model, model->COMMENT);
    vbox->addWidget(view);

    setLayout(vbox);
}
