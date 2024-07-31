#pragma once
#include <QTableView>

class TableView: public QTableView {
public:
    TableView();

private:
    void cutSelection();
    void copySelection();
    void pasteToSelection();
    void deleteSelection();
};
