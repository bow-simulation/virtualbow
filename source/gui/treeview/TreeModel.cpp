#include "TreeModel.hpp"
#include "gui/viewmodel/DataViewModel.hpp"

#include <QDebug>
/*
TreeModel::TreeModel(DataViewModel* model)
    : model(model) {

}

int TreeModel::rowCount(const QModelIndex& parent) const {
    qInfo() << "rowCount for" << parent;

    if(parent.isValid()) {
        if(parent.column() > 0) {
            return 0;
        }
        if(parent.row() == ROW_MATERIALS) {
            qInfo() << "Rows:" << model->getData().materials.size();
            return model->getData().materials.size();
        }
        if(parent.row() == ROW_PROFILE) {
            qInfo() << "Rows:" << model->getData().profile.size();
            return model->getData().profile.size();
        }
    }

    qInfo() << "Rows:" << N_TOP_LEVEL_ROWS;
    return N_TOP_LEVEL_ROWS;
}

int TreeModel::columnCount(const QModelIndex& parent) const {
    return 1;
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex& parent) const {

    //if(!hasIndex(row, column, parent)) {
    //    return QModelIndex();
    /}


    if(!parent.isValid()) {
        return createIndex(row, column, (quintptr) parent.row());
    }
    else {
        return createIndex(row, column, (quintptr) -1);
    }

    return QModelIndex();
}

QModelIndex TreeModel::parent(const QModelIndex& index) const {
    if(!index.isValid()) {
        return QModelIndex();
    }

    int id = index.internalId();
    if(id == -1) {
        return QModelIndex();
    }

    return createIndex(id, 0, (quintptr) -1);
}

QVariant TreeModel::data(const QModelIndex& index, int role) const {
    if(index.isValid() && role == Qt::DisplayRole) {
        return "Test";
    }

    return QVariant();


    qInfo() << "Internal ID:" << index.internalId();

    if(role == Qt::DisplayRole) {
        switch(index.row()) {
            case ROW_COMMENTS:
                return "Comments";

            case ROW_SETTINGS:
                return "Settings";

            case ROW_MATERIALS:
                return "Materials";

            case ROW_DIMENSIONS:
                return "Dimensions";

            case ROW_PROFILE:
                return "Profile";

            case ROW_WIDTH:
                return "Width";

            case ROW_LAYERS:
                return "Layers";

            case ROW_STRING:
                return "String";

            case ROW_MASSES:
                return "Masses";

            case ROW_DAMPING:
                return "Damping";
        }
    }

    if(role == Qt::DecorationRole) {
        switch(index.row()) {
            case ROW_COMMENTS:
                return QIcon(":/icons/model-comments.svg");

            case ROW_SETTINGS:
                return QIcon(":/icons/model-settings.svg");

            case ROW_MATERIALS:
                return QIcon(":/icons/model-materials.svg");

            case ROW_DIMENSIONS:
                return QIcon(":/icons/model-dimensions.svg");

            case ROW_PROFILE:
                return QIcon(":/icons/model-profile.svg");

            case ROW_WIDTH:
                return QIcon(":/icons/model-width.svg");

            case ROW_LAYERS:
                return QIcon(":/icons/model-layers.svg");

            case ROW_STRING:
                return QIcon(":/icons/model-string.svg");

            case ROW_MASSES:
                return QIcon(":/icons/model-masses.svg");

            case ROW_DAMPING:
                return QIcon(":/icons/model-damping.svg");
        }
    }

    return QVariant();

}
*/
