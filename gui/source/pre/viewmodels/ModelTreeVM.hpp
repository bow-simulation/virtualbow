#pragma once
#include "solver/BowModel.hpp"
#include <QAbstractItemModel>
#include <QStringList>

class ModelTreeVM: public QAbstractItemModel {
    Q_OBJECT

public:
    ModelTreeVM(QObject *parent = nullptr);

    // Abstract method implementations

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;

    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
};

/*
class ModelTreeVM: public QAbstractItemModel {
public:
    const BowModel& getBowModel() const {
        return model;
    }

    void setBowModel(const BowModel& model) {
        this->model = model;
    }



    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        return 1;
    }

    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override {
        return 1;
    }

    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override {
        if(!hasIndex(row, column, parent)) {
            return QModelIndex();
        }

        if(row == 0 && column == 0) {
            return createIndex(row, column);
        }

        return QModelIndex();
    }

    virtual QModelIndex parent(const QModelIndex &index) const override {
        return QModelIndex();
    }

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
        return "Hello World";
    }

private:
    BowModel model;
};
*/
