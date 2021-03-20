#include "TableEditor.hpp"
#include <algorithm>

TableEditor::TableEditor(const QList<QString>& labels, const QList<const UnitGroup*>& units)
    : model(labels, units, 100)
{
    setModel(&model);

    QObject::connect(&model, &TableModel::modified, this, &TableEditor::modified);
    QObject::connect(this->selectionModel(), &QItemSelectionModel::selectionChanged, this, [&] {
        emit this->rowSelectionChanged(getSelectedIndices());
    });
}

MatrixXd TableEditor::getData() const {
    return model.getData();
}

void TableEditor::setData(const MatrixXd& data) {
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
