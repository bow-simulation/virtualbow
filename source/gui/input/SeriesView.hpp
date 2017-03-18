#pragma once
#include "model/Document.hpp"
#include <QtWidgets>

class Series;

class SeriesView: public QWidget
{
public:
    SeriesView(const QString& lb_args, const QString& lb_vals, DocItem<Series>& doc_item);

private:
    DocItem<Series>& doc_item;
    Connection connection;
    QTableWidget* table;

    virtual void keyPressEvent(QKeyEvent* event) override;

    void setData(const Series& series);
    void setCellValue(int i, int j, double value);
    double getCellValue(int i, int j) const;

    void insertRow(bool above);
    void deleteLastRow();
    bool deleteSelectedRows();
};
