#include "TableEditor.hpp"
#include <algorithm>

TableEditor::TableEditor(const QList<QString>& labels, const QList<const UnitGroup*>& units)
    : model(labels, units, 100)
{
    setModel(&model);
    QObject::connect(&model, &TableModel::modified, this, &TableEditor::modified);
}

MatrixXd TableEditor::getData() const {
    return model.getData();
}

void TableEditor::setData(const MatrixXd& data) {
    model.setData(data);
}

QVector<int> TableEditor::getSelectedIndices() {
    /*
    QVector<int> selection;
    int index = 0;
    for(int i = 0; i < this->rowCount(); ++i) {
        bool arg_valid, val_valid;
        QLocale().toDouble(this->item(i, 0)->text(), &arg_valid);
        QLocale().toDouble(this->item(i, 1)->text(), &val_valid);

        if(arg_valid && val_valid) {
            if(this->item(i, 0)->isSelected() || this->item(i, 1)->isSelected()) {
                selection.push_back(index);
            }
            ++index;
        }
    }

    return selection;
    */

    return QVector<int>();
}
