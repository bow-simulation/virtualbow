#pragma once
#include "numerics/Series.hpp"
#include <QtWidgets>

class TableItem: public QTableWidgetItem
{
    virtual void setData(int role, const QVariant& data);
};

class SeriesEditor: public QTableWidget
{
    Q_OBJECT

public:
    SeriesEditor(const QString& x_label, const QString& y_label, int rows);
    Series getData() const;
    void setData(const Series& data);
    qreal limb_length = {1.0};

signals:
    void modified();

private:
    void cutSelection();
    void copySelection();
    void pasteToSelection();
    void deleteSelection();
};

