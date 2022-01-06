#include "TableEditor.hpp"
#include "TableDelegate.hpp"
#include <algorithm>

TableEditor::TableEditor(const QString& x_label, const QString& y_label, const UnitGroup& x_unit, const UnitGroup& y_unit)
    : model(x_label, y_label, x_unit, y_unit)
{
    setModel(&model);
    setItemDelegate(new TableDelegate());

    QObject::connect(&model, &TableModel::modified, this, &TableEditor::modified);
    QObject::connect(this->selectionModel(), &QItemSelectionModel::selectionChanged, this, [&] {
        emit this->rowSelectionChanged(getSelectedIndices());
    });
}

std::vector<Vector<2>> TableEditor::getData() const {
    return model.getData();
}

void TableEditor::setData(const std::vector<Vector<2>>& data) {
    model.setData(data);
}

QVector<int> TableEditor::getSelectedIndices() {
    QItemSelectionRange range = selectionModel()->selection().first();
    QVector<int> selection;
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

    return selection;
}
