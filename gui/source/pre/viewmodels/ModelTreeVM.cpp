#include "ModelTreeVM.hpp"
#include <QIcon>

// Row indices of the top-level items in the model tree
enum TopLevelRow {
    COMMENTS = 0,
    SETTINGS = 1,
    DIMENSIONS = 2,
    MATERIALS = 3,
    LAYERS = 4,
    PROFILE = 5,
    WIDTH = 6,
    STRING = 7,
    MASSES = 8,
    DAMPING = 9,

    FIRST = COMMENTS,
    LAST  = DAMPING
};

QString topLevelItemName(int row) {
    switch(row) {
        case TopLevelRow::COMMENTS: return "Comments";
        case TopLevelRow::SETTINGS: return "Settings";
        case TopLevelRow::DIMENSIONS: return "Dimensions";
        case TopLevelRow::MATERIALS: return "Materials";
        case TopLevelRow::LAYERS: return "Layers";
        case TopLevelRow::PROFILE: return "Profile";
        case TopLevelRow::WIDTH: return "Width";
        case TopLevelRow::STRING: return "String";
        case TopLevelRow::MASSES: return "Masses";
        case TopLevelRow::DAMPING: return "Damping";
        default: throw std::invalid_argument("Unknown enum variant");
    }
}

QString topLevelToolTip(int row) {
    switch(row) {
        case TopLevelRow::COMMENTS: return "Information about this bow model";
        case TopLevelRow::SETTINGS: return "Settings that control how the bow is simulated";
        case TopLevelRow::DIMENSIONS: return "General dimensions like brace height, draw length and handle geometry";
        case TopLevelRow::MATERIALS: return "List of materials used in the bow's limbs";
        case TopLevelRow::LAYERS: return "List of layers that make up the bow's limbs";
        case TopLevelRow::PROFILE: return "List of curve segments that define the initial profile of the bow";
        case TopLevelRow::WIDTH: return "Width variation of the bow limbs";
        case TopLevelRow::STRING: return "Properties of the bowstring";
        case TopLevelRow::MASSES: return "Mass of the arrow and other components";
        case TopLevelRow::DAMPING: return "Damping properties of limbs and string";
        default: throw std::invalid_argument("Unknown enum variant");
    }
}

QIcon topLevelItemIcon(int row) {
    switch(row) {
        case TopLevelRow::COMMENTS: return QIcon(":/icons/model-comments.svg");
        case TopLevelRow::SETTINGS: return QIcon(":/icons/model-settings.svg");
        case TopLevelRow::DIMENSIONS: return QIcon(":/icons/model-dimensions.svg");
        case TopLevelRow::MATERIALS: return QIcon(":/icons/model-materials.svg");
        case TopLevelRow::LAYERS: return QIcon(":/icons/model-layers.svg");
        case TopLevelRow::PROFILE: return QIcon(":/icons/model-profile.svg");
        case TopLevelRow::WIDTH: return QIcon(":/icons/model-width.svg");
        case TopLevelRow::STRING: return QIcon(":/icons/model-string.svg");
        case TopLevelRow::MASSES: return QIcon(":/icons/model-masses.svg");
        case TopLevelRow::DAMPING: return QIcon(":/icons/model-damping.svg");
        default: throw std::invalid_argument("Unknown enum variant");
    }
}

ModelTreeVM::ModelTreeVM(QObject *parent): QAbstractItemModel(parent) {

}

QModelIndex ModelTreeVM::index(int row, int column, const QModelIndex &parent) const {
    if (parent.isValid() || row < TopLevelRow::FIRST || row > TopLevelRow::LAST || column != 0) {
        return QModelIndex();
    }

    return createIndex(row, column);
}

QModelIndex ModelTreeVM::parent(const QModelIndex &index) const {
    Q_UNUSED(index);
    return QModelIndex();
}

int ModelTreeVM::rowCount(const QModelIndex &parent) const {
    if (parent.isValid()) {
        return 0;
    }

    return TopLevelRow::LAST + 1;
}

int ModelTreeVM::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return 1;
}

QVariant ModelTreeVM::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() > TopLevelRow::LAST) {
        return QVariant();
    }

    switch(role) {
        case Qt::DisplayRole: return topLevelItemName(index.row());
        case Qt::ToolTipRole: return topLevelToolTip(index.row());
        case Qt::DecorationRole: return topLevelItemIcon(index.row());

        default: return QVariant();
    }
}
