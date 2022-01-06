#include "TableEditor.hpp"
#include "TableDelegate.hpp"
#include <algorithm>

TableEditor::TableEditor(const QString& x_label, const QString& y_label, const TableSpinnerOptions& x_options, const TableSpinnerOptions& y_options)
    : model(x_label, y_label, x_options.units, y_options.units)
{
    setModel(&model);
    setItemDelegateForColumn(0, new TableDelegate(x_options));
    setItemDelegateForColumn(1, new TableDelegate(y_options));

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
