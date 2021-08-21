#include "ProfileEditor.hpp"
#include <algorithm>


class Header: public QHeaderView {
public:
    Header(QWidget* parent)
        : QHeaderView(Qt::Horizontal, parent)
    {
        auto button_add = new QToolButton();
        button_add->setIcon(QIcon(":/icons/list-add.svg"));

        auto button_remove = new QToolButton();
        button_remove->setIcon(QIcon(":/icons/list-remove.svg"));

        auto hbox = new QHBoxLayout();
        hbox->setMargin(2);
        hbox->setSpacing(2);
        hbox->addStretch();
        hbox->addWidget(button_add);
        hbox->addWidget(button_remove);

        this->setLayout(hbox);
    }
};

ProfileEditor::ProfileEditor(const UnitSystem& units) {
    auto list = new QTreeWidget();
    list->header()->setSectionResizeMode(QHeaderView::Stretch);
    list->header()->setStretchLastSection(false);
    list->setHeader(new Header(this));
    list->setHeaderLabel("Segments");
    list->setRootIsDecorated(false);

    auto item0 = new QTreeWidgetItem({"1 - Line"});
    item0->setIcon(0, QIcon(":/icons/segment-line.svg"));

    auto item1 = new QTreeWidgetItem({"2 - Arc"});
    item1->setIcon(0, QIcon(":/icons/segment-arc.svg"));

    auto item2 = new QTreeWidgetItem({"3 - Spiral"});
    item2->setIcon(0, QIcon(":/icons/segment-spiral.svg"));

    list->addTopLevelItem(item0);
    list->addTopLevelItem(item1);
    list->addTopLevelItem(item2);

    auto table = new QTableWidget(3, 2);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->horizontalHeader()->setVisible(false);
    table->verticalHeader()->setVisible(false);
    table->setCellWidget(0, 0, new QComboBox());
    table->setCellWidget(1, 0, new QComboBox());
    table->setCellWidget(2, 0, new QComboBox());

    auto vbox = new QVBoxLayout();
    vbox->addWidget(list, 1);
    vbox->addWidget(table, 0);

    this->setLayout(vbox);

    /*
    QObject::connect(&model, &TableModel::modified, this, &ProfileEditor::modified);
    QObject::connect(this->selectionModel(), &QItemSelectionModel::selectionChanged, this, [&] {
        emit this->rowSelectionChanged(getSelectedIndices());
    });
    */
}

MatrixXd ProfileEditor::getData() const {
    return MatrixXd(); //model.getData();
}

void ProfileEditor::setData(const MatrixXd& data) {
    //model.setData(data);
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
