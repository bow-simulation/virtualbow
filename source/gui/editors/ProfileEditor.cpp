#include "ProfileEditor.hpp"
#include "ProfileSegmentEditor.hpp"
#include "solver/model/ProfileCurve.hpp"
#include "gui/widgets/DoubleSpinBox.hpp"
#include "gui/units/UnitSystem.hpp"
#include <algorithm>

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

    /*
    QObject::connect(&model, &TableModel::modified, this, &ProfileEditor::modified);
    QObject::connect(this->selectionModel(), &QItemSelectionModel::selectionChanged, this, [&] {
        emit this->rowSelectionChanged(getSelectedIndices());
    });
    */
}

std::vector<SegmentInput> ProfileEditor::getData() const {
    return profile_tree->getData();
}

void ProfileEditor::setData(const std::vector<SegmentInput>& data) {
    profile_tree->setData(data);
}

QVector<int> ProfileEditor::getSelectedIndices() {
    //QItemSelectionRange range = selectionModel()->selection().first();
    QVector<int> selection;
    /*
    int index = 0;
    for(int i = 0; i < model.rowCount(); ++i) {
        QVariant arg = model.index(i, 0).data();
        QVariant val = model.index(i, 1).data();
        if(!arg.isNull() && !val.isNull()) {
            if(i >= range.top() && i <= range.bottom()) {
                selection.push_back(index);
            }
            ++index;
        }
    }
    */

    return selection;
}
