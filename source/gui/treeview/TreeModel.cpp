#include "TreeModel.hpp"

TreeModel::TreeModel(ViewModel* model)
    : model(model) {

}

int TreeModel::rowCount(const QModelIndex& parent) const {
    return N_ROWS;
}

int TreeModel::columnCount(const QModelIndex& parent) const {
    return 1;
}

QVariant TreeModel::data(const QModelIndex& index, int role) const {
    if(!index.isValid()) {
        return QVariant();
    }

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
                return QIcon(":/icons/model-settings.svg");

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
