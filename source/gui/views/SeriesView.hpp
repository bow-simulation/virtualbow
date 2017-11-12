#pragma once
#include "model/document/Document.hpp"
#include <QtWidgets>

class Series;

class SeriesView: public QWidget
{
    Q_OBJECT

public:
    SeriesView(const QString& lb_args, const QString& lb_vals, DocumentItem<Series>& doc_item);

signals:
    void selectionChanged(const std::vector<int>& selected_rows);

private:
    DocumentItem<Series>& doc_item;
    QTableWidget* table;

    virtual void keyPressEvent(QKeyEvent* event) override;

    void setData(const Series& series);
    void setCellValue(int i, int j, double value);
    double getCellValue(int i, int j) const;

    void insertRow(bool above);
    void deleteLastRow();
    bool deleteSelectedRows();
};
