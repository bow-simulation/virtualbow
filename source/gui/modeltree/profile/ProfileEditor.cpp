#include "ProfileEditor.hpp"
#include "ProfileTreeView.hpp"
#include "SegmentEditor.hpp"
#include <algorithm>
#include <QTableWidget>
#include <QSplitter>
#include <QVBoxLayout>

#include <QLabel>

ProfileEditor::ProfileEditor()
    : profile_tree(new ProfileTreeView())
{
    auto splitter = new QSplitter(Qt::Vertical);
    splitter->setChildrenCollapsible(false);
    splitter->addWidget(profile_tree);
    splitter->addWidget(new QLabel("Editor"));
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 0);

    auto vbox = new QVBoxLayout();
    this->setLayout(vbox);
    vbox->setMargin(0);
    vbox->addWidget(splitter);

    /*
    QObject::connect(profile_tree, &ProfileTree::itemSelectionChanged, this, [&, splitter](){
        splitter->replaceWidget(1, profile_tree->getSelectedEditor());
    });

    QObject::connect(profile_tree, &ProfileTree::modified, this, &ProfileEditor::modified);
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
    return profile_tree->getData();
}

void ProfileEditor::setData(const ProfileInput& data) {
    profile_tree->setData(data);
}
