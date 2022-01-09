#include "ProfileEditor.hpp"
#include "ProfileTreeModel.hpp"
#include "ProfileTreeView.hpp"
#include "ProfileSegmentView.hpp"
#include "SegmentEditor.hpp"
#include <algorithm>
#include <QTableWidget>
#include <QSplitter>
#include <QVBoxLayout>

#include <QLabel>
#include <QDebug>

ProfileEditor::ProfileEditor()
    : profile_model(new ProfileTreeModel()),
      profile_view(new ProfileTreeView(profile_model)),
      segment_view(new ProfileSegmentView(profile_model))
{    
    auto splitter = new QSplitter(Qt::Vertical);
    splitter->setChildrenCollapsible(false);
    splitter->addWidget(profile_view);
    splitter->addWidget(segment_view);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 0);

    auto vbox = new QVBoxLayout();
    this->setLayout(vbox);
    vbox->setMargin(0);
    vbox->addWidget(splitter);

    QObject::connect(profile_view->selectionModel(), &QItemSelectionModel::selectionChanged, this, [=] {
        auto selection = profile_view->selectionModel()->selectedIndexes();
        if(selection.size() == 1) {
            segment_view->showEditor(selection[0]);
        }
        else {
            segment_view->hideEditor();
        }
    });

    /*
    QObject::connect(profile_view->selectionModel(), &QItemSelectionModel::selectionChanged, this, [=] {
        auto selection = profile_view->selectionModel()->selectedIndexes();
        if(selection.size() == 1) {
            SegmentInput segment = profile_model->getData(selection[0]);
            auto editor = createSegmentEditor(segment);
            splitter->replaceWidget(1, editor);
        }
        else {
            qInfo() << "Show no editor";
            splitter->replaceWidget(1, new QLabel("Editor"));
        }
    });
    */

    // QObject::connect(tree_model, &ProfileTreeModel::modified, this, &ProfileEditor::modified);

    /*
    QObject::connect(profile_tree, &ProfileTree::itemSelectionChanged, this, [&, splitter](){
        splitter->replaceWidget(1, profile_tree->getSelectedEditor());
    });
    QObject::connect(profile_tree->selectionModel(), &QItemSelectionModel::selectionChanged, this, [&] {
        QList<int> indices;
        for(QModelIndex index: profile_tree->selectionModel()->selectedRows()) {
            indices.push_back(index.row());
        }
        emit selectionChanged(indices);
    });
    */
}

ProfileInput ProfileEditor::getData() const {
    return profile_model->getData();
}

void ProfileEditor::setData(const ProfileInput& data) {
    profile_model->setData(data);
}
