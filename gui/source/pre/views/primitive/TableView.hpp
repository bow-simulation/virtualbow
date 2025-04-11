#pragma once
#include <QTableView>

class TableView2: public QTableView {
public:
    TableView2();

private:
    void cutSelection();
    void copySelection();
    void pasteToSelection();
    void deleteSelection();
};
