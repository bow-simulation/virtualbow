#pragma once
#include <QTreeView>

class ViewModel;

class TreeModel: public QAbstractTableModel {
    Q_OBJECT

public:
    static const int N_TOP_LEVEL_ROWS = 9;
    static const int ROW_COMMENTS = 0;
    static const int ROW_SETTINGS = 1;
    static const int ROW_MATERIALS = 2;
    static const int ROW_DIMENSIONS = 3;
    static const int ROW_PROFILE = 4;
    static const int ROW_WIDTH = 5;
    static const int ROW_LAYERS = 6;
    static const int ROW_STRING = 7;
    static const int ROW_MASSES = 8;
    static const int ROW_DAMPING = 9;

    TreeModel(ViewModel* model);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

private:
    ViewModel* model;
};
