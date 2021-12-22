#include "ProfileEditor.hpp"
#include "ProfileTree.hpp"
#include "SegmentEditor.hpp"
#include <algorithm>

#include <QTableWidget>
#include <QSplitter>
#include <QVBoxLayout>

ProfileEditor::ProfileEditor()
    : profile_tree(new ProfileTree())
{
    auto placeholder = new QTableWidget();
    placeholder->horizontalHeader()->setVisible(false);
    placeholder->verticalHeader()->setVisible(false);

    auto splitter = new QSplitter(Qt::Vertical);
    splitter->setChildrenCollapsible(false);
    splitter->addWidget(profile_tree);
    splitter->addWidget(placeholder);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 0);

    auto vbox = new QVBoxLayout();
    vbox->setMargin(0);
    vbox->addWidget(splitter);

    this->setLayout(vbox);

    QObject::connect(profile_tree, &QTreeWidget::itemSelectionChanged, this, [&, splitter, placeholder](){
        auto selection = profile_tree->selectedItems();
        if(selection.isEmpty()) {
            splitter->replaceWidget(1, placeholder);
        }
        else if(selection.size() == 1) {
            auto item = dynamic_cast<ProfileTreeItem*>(selection[0]);
            splitter->replaceWidget(1, item->getEditor());
        }
    });

    QObject::connect(profile_tree, &ProfileTree::modified, this, &ProfileEditor::modified);
    QObject::connect(profile_tree->selectionModel(), &QItemSelectionModel::selectionChanged, this, [&] {
        QList<int> indices;
        for(QModelIndex index: profile_tree->selectionModel()->selectedRows()) {
            indices.push_back(index.row());
        }
        emit selectionChanged(indices);
    });
}

ProfileInput ProfileEditor::getData() const {
    return profile_tree->getData();
}

void ProfileEditor::setData(const ProfileInput& data) {
    profile_tree->setData(data);
}
