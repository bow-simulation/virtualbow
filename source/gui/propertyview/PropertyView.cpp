#include "PropertyView.hpp"
#include "gui/viewmodel/DataViewModel.hpp"
#include "CommentView.hpp"
#include <QLabel>

PropertyView::PropertyView(DataViewModel* model) {
    QObject::connect(model, &DataViewModel::nothingSelected, this, [&]{
        setCurrentWidget(placeholder);
    });

    QObject::connect(model, &DataViewModel::commentsSelected, this, [&, model]{
        if(comment_view == nullptr) {
            comment_view = new CommentView(model);
            addWidget(comment_view);
        }
        setCurrentWidget(comment_view);
    });

    QObject::connect(model, &DataViewModel::settingsSelected, this, [&]{
        if(settings_view == nullptr) {
            settings_view = new QLabel("Settings");
            addWidget(settings_view);
        }
        setCurrentWidget(settings_view);
    });

    QObject::connect(model, &DataViewModel::dimensionsSelected, this, [&]{
        if(dimensions_view == nullptr) {
            dimensions_view = new QLabel("Dimensions");
            addWidget(dimensions_view);
        }
        setCurrentWidget(dimensions_view);
    });

    this->addWidget(placeholder);
    this->setCurrentWidget(placeholder);

    /*
    QObject::connect(model, &ViewModel::materialsSelected, this, [label]{
        label->setText("Materials");
    });

    QObject::connect(model, &ViewModel::layersSelected, this, [label]{
        label->setText("Layers");
    });

    QObject::connect(model, &ViewModel::profileSelected, this, [label]{
        label->setText("Profile");
    });

    QObject::connect(model, &ViewModel::widthSelected, this, [label]{
        label->setText("Width");
    });

    QObject::connect(model, &ViewModel::stringSelected, this, [label]{
        label->setText("String");
    });

    QObject::connect(model, &ViewModel::massesSelected, this, [label]{
        label->setText("Masses");
    });

    QObject::connect(model, &ViewModel::dampingSelected, this, [label]{
        label->setText("Damping");
    });

    auto vbox = new QVBoxLayout();
    vbox->addWidget(label);

    this->setLayout(vbox);
    */
}
