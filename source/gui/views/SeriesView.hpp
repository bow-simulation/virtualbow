#pragma once
#include "bow/document/Document.hpp"
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
    //TableWidget* table;

    virtual void keyPressEvent(QKeyEvent* event) override;

    void update_value();
    void update_error();

    void insertRow(bool above);
    void deleteLastRow();
    bool deleteSelectedRows();
};
