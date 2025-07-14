#pragma once
#include <QPushButton>

class QAbstractItemModel;

class ColorView: public QPushButton {
    Q_OBJECT

public:
    ColorView(QAbstractItemModel* model, QPersistentModelIndex index, const QString& tooltip);

private:
    void setColor(const QColor& color);
};
