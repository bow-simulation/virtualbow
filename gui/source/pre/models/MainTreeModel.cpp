#include "MainTreeModel.hpp"
#include "pre/utils/ListUtils.hpp"
#include <QModelIndex>
#include <QIcon>
#include <algorithm>

MainTreeModel::MainTreeModel():
    bow(nullptr)
{
    // Emit modification signals on changes to data and tree structure
    QObject::connect(this, &QAbstractItemModel::dataChanged, this, &MainTreeModel::contentModified);
    QObject::connect(this, &QAbstractItemModel::rowsInserted, this, &MainTreeModel::contentModified);
    QObject::connect(this, &QAbstractItemModel::rowsRemoved, this, &MainTreeModel::contentModified);
    QObject::connect(this, &QAbstractItemModel::rowsMoved, this, &MainTreeModel::contentModified);
}

void MainTreeModel::setBowModel(BowModel* bow) {
    beginResetModel();
    this->bow = bow;
    endResetModel();
}

bool MainTreeModel::canInsertMaterial(const QModelIndexList& indexes) {
    if(indexes.size() == 1) {
        if(indexes[0].internalId() == ItemType::MATERIAL) {
            return true;    // Single material selected
        }

        if(indexes[0].internalId() == ItemType::TOPLEVEL && indexes[0].row() == TopLevelItem::MATERIALS) {
            return true;    // Material category selected
        }
    }

    return false;
}

void MainTreeModel::insertMaterial(int row) {
    if(row < 0 || row > bow->section.materials.size()) {
        throw std::invalid_argument("Invalid material index for insertion");
    }

    Material material {
        .name = bow->generateMaterialName(),
        .color = "#d0b391",
        .density = 675.0,
        .youngs_modulus = 12e9,
        .shear_modulus = 6e9
    };

    QModelIndex parent = createIndex(TopLevelItem::MATERIALS, 0, ItemType::TOPLEVEL);
    beginInsertRows(parent, row, row);

    auto position = std::next(bow->section.materials.begin(), row);
    bow->section.materials.insert(position, material);

    endInsertRows();
}

void MainTreeModel::appendMaterial() {
    insertMaterial(bow->section.materials.size());
}

bool MainTreeModel::canInsertLayer(const QModelIndexList& indexes) {
    if(indexes.size() == 1) {
        if(indexes[0].internalId() == ItemType::LAYER) {
            return true;    // Single layer selected
        }

        if(indexes[0].internalId() == ItemType::TOPLEVEL && indexes[0].row() == TopLevelItem::LAYERS) {
            return true;    // Layer category selected
        }
    }

    return false;
}

void MainTreeModel::insertLayer(int row) {
    if(row < 0 || row > bow->section.layers.size()) {
        throw std::invalid_argument("Invalid layer index for insertion");
    }

    Layer layer {
        .name = bow->generateLayerName(),
        .material = bow->section.materials.empty() ? "" : bow->section.materials.front().name,
        .height = {{0.0, 0.01}, {1.0, 0.01}}
    };

    QModelIndex parent = createIndex(TopLevelItem::LAYERS, 0, ItemType::TOPLEVEL);
    beginInsertRows(parent, row, row);

    auto position = std::next(bow->section.layers.begin(), row);
    bow->section.layers.insert(position, layer);

    endInsertRows();
}

void MainTreeModel::appendLayer() {
    insertLayer(bow->section.layers.size());
}

bool MainTreeModel::canInsertSegment(const QModelIndexList& indexes) {
    if(indexes.size() == 1) {
        if(indexes[0].internalId() == ItemType::SEGMENT) {
            return true;    // Single segment selected
        }

        if(indexes[0].internalId() == ItemType::TOPLEVEL && indexes[0].row() == TopLevelItem::PROFILE) {
            return true;    // Profile category selected
        }
    }

    return false;
}

void MainTreeModel::insertSegment(int row, SegmentType type) {
    if(row < 0 || row > bow->profile.segments.size()) {
        throw std::invalid_argument("Invalid segment index for insertion");
    }

    QModelIndex parent = createIndex(TopLevelItem::PROFILE, 0, ItemType::TOPLEVEL);
    beginInsertRows(parent, row, row);

    auto position = std::next(bow->profile.segments.begin(), row);
    bow->profile.segments.insert(position, createDefaultSegment(type));

    endInsertRows();
}

void MainTreeModel::appendSegment(SegmentType type) {
    insertSegment(bow->profile.segments.size(), type);
}

// Indexes as a whole can be removed if none of them refers to a top-level item
bool MainTreeModel::canRemoveIndexes(QModelIndexList& indexes) {
    for(QModelIndex index: indexes) {
        if(index.internalId() == ItemType::TOPLEVEL) {
            return false;
        }
    }

    return true;
}

void MainTreeModel::removeIndexes(QModelIndexList indexes) {
    // Sort in reverse order by row  because the ones further down need to be deleted first
    std::sort(indexes.begin(), indexes.end(), [](const auto& lhs, const auto& rhs){
        return lhs.row() > rhs.row();
    });

    for(QModelIndex index: indexes) {
        switch(index.internalId()) {
        case ItemType::MATERIAL:
            removeMaterial(index.row());
            break;
        case ItemType::LAYER:
            removeLayer(index.row());
            break;
        case ItemType::SEGMENT:
            removeSegment(index.row());
            break;
        }
    }
}

void MainTreeModel::removeMaterial(int row) {
    if(row < 0 || row >= bow->section.materials.size()) {
        throw std::invalid_argument("Invalid material index for removal");
    }

    QModelIndex parent = createIndex(TopLevelItem::MATERIALS, 0, ItemType::TOPLEVEL);
    beginRemoveRows(parent, row, row);

    // Remove material
    // Layers that refer to the material will become invalid
    auto position = std::next(bow->section.materials.begin(), row);
    bow->section.materials.erase(position);

    endRemoveRows();
}

void MainTreeModel::removeLayer(int row) {
    if(row < 0 || row >= bow->section.layers.size()) {
        throw std::invalid_argument("Invalid layer index for removal");
    }

    QModelIndex parent = createIndex(TopLevelItem::LAYERS, 0, ItemType::TOPLEVEL);
    beginRemoveRows(parent, row, row);

    // Remove layer
    // Bow will become invalid when the last layer is removed
    auto position = std::next(bow->section.layers.begin(), row);
    bow->section.layers.erase(position);

    endRemoveRows();
}

void MainTreeModel::removeSegment(int row) {
    if(row < 0 || row >= bow->profile.segments.size()) {
        throw std::invalid_argument("Invalid segment index for removal");
    }

    QModelIndex parent = createIndex(TopLevelItem::PROFILE, 0, ItemType::TOPLEVEL);
    beginRemoveRows(parent, row, row);

    // Remove segment
    // Bow will become invalis when the last segment is removed
    auto position = std::next(bow->profile.segments.begin(), row);
    bow->profile.segments.erase(position);

    endRemoveRows();
}

// Indexes can be moved up if one or more sub-level items of the same category are selected and the first row is unselected
bool MainTreeModel::canMoveIndexesUp(const QModelIndexList& indexes) {
    // Can't move empty selection or one that starts with a top-level item
    if(indexes.isEmpty() || indexes.front().internalId() == ItemType::TOPLEVEL) {
        return false;
    }

    // First item is not top-level => check if other items are of the same sub-level type
    for(int i = 1; i < indexes.size(); ++i) {
        if(indexes[i].internalId() != indexes[0].internalId()) {
            return false;
        }
    }

    // Verify that the first row is not contained in the indexes
    for(QModelIndex index: indexes) {
        if(index.row() == 0) {
            return false;
        }
    }

    return true;
}

// Indexes can be moved down if one or more sub-level items of the same category are selected and the last row is unselected
bool MainTreeModel::canMoveIndexesDown(const QModelIndexList& indexes) {
    // Can't move empty selection or one that starts with a top-level item
    if(indexes.isEmpty() || indexes.front().internalId() == ItemType::TOPLEVEL) {
        return false;
    }

    // First item is not top-level => check if other items are of the same sub-level type
    for(int i = 1; i < indexes.size(); ++i) {
        if(indexes[i].internalId() != indexes[0].internalId()) {
            return false;
        }
    }

    // Last row index of the parent node
    QModelIndex parent = indexes.first().parent();
    int lastIndex = rowCount(parent) - 1;

    // Verify that the last row is not contained in the indexes
    for(QModelIndex index: indexes) {
        if(index.row() == lastIndex) {
            return false;
        }
    }

    return true;
}

void MainTreeModel::moveIndexesUp(QModelIndexList indexes) {
    // Sort the indexes by row because the order of swapping makes a difference
    std::sort(indexes.begin(), indexes.end(), [](const auto& lhs, const auto& rhs){
        return lhs.row() < rhs.row();
    });

    // Swap the item at each index with the one before it
    for(QModelIndex index: indexes) {
        switch(index.internalId()) {
        case ItemType::MATERIAL:
            swapMaterials(index.row(), index.row() - 1);
            break;
        case ItemType::LAYER:
            swapLayers(index.row(), index.row() - 1);
            break;
        case ItemType::SEGMENT:
            swapSegments(index.row(), index.row() - 1);
            break;
        }
    }
}

void MainTreeModel::moveIndexesDown(QModelIndexList indexes) {
    // Sort in reverse order by row  because the order of swapping makes a difference
    std::sort(indexes.begin(), indexes.end(), [](const auto& lhs, const auto& rhs){
        return lhs.row() > rhs.row();
    });

    // Swap the item at each index with the one after it
    for(QModelIndex index: indexes) {
        switch(index.internalId()) {
        case ItemType::MATERIAL:
            swapMaterials(index.row(), index.row() + 1);
            break;
        case ItemType::LAYER:
            swapLayers(index.row(), index.row() + 1);
            break;
        case ItemType::SEGMENT:
            swapSegments(index.row(), index.row() + 1);
            break;
        }
    }
}

void MainTreeModel::swapMaterials(int i, int j) {
    if(i == j || i < 0 || j < 0 ||  i >= bow->section.materials.size() || j >= bow->section.materials.size()) {
        throw std::invalid_argument("Invalid material indices for swapping");
    }

    // Indices must be in ascending order
    if(i > j) {
        std::swap(i, j);
    }

    QModelIndex parent = createIndex(TopLevelItem::MATERIALS, 0, ItemType::TOPLEVEL);
    beginMoveRows(parent, i, i, parent, j);
    beginMoveRows(parent, j, j, parent, i);

    // Swap the two materials
    // Layers that refer to the materials stay valid since they refer to them by name
    swapListNodes(bow->section.materials, i, j);

    /*
    auto it1 = std::next(bow->section.materials.begin(), i);
    auto it2 = std::next(bow->section.materials.begin(), j);
    //std::swap(*it1, *it2);


    auto after2 = std::next(it2);
    bow->section.materials.splice(it1, bow->section.materials, it2);    // Move it2 before it1
    bow->section.materials.splice(after2, bow->section.materials, it1);    // Move it1 (now after it2) to afterIt2
    */

    endMoveRows();
}

void MainTreeModel::swapLayers(int i, int j) {
    if(i == j || i < 0 || j < 0 || i >= bow->section.layers.size() || j >= bow->section.layers.size()) {
        throw std::invalid_argument("Invalid layer indices for swapping");
    }

    QModelIndex parent = createIndex(TopLevelItem::LAYERS, 0, ItemType::TOPLEVEL);
    beginMoveRows(parent, i, i, parent, j);
    beginMoveRows(parent, j, j, parent, i);

    // Swap the two layers
    swapListNodes(bow->section.layers, i, j);

    endMoveRows();
}

void MainTreeModel::swapSegments(int i, int j) {
    if(i == j || i < 0 || j < 0 || i >= bow->profile.segments.size() || j >= bow->profile.segments.size()) {
        throw std::invalid_argument("Invalid segment indices for swapping");
    }

    QModelIndex parent = createIndex(TopLevelItem::PROFILE, 0, ItemType::TOPLEVEL);
    beginMoveRows(parent, i, i, parent, j);
    beginMoveRows(parent, j, j, parent, i);

    // Swap the two segments
    swapListNodes(bow->profile.segments, i, j);

    endMoveRows();
}

QModelIndex MainTreeModel::index(int row, int column, const QModelIndex &parent) const {
    // We only are about the first and only column
    if(column != 0) {
        return QModelIndex();
    }

    // If the parent is invalid we are at the top level. Return a model index of type "top level" if the row and column is also in a valid range.
    if(!parent.isValid()) {
        return createIndex(row, column, ItemType::TOPLEVEL);
    }

    // If the parent is valid, we are at a leaf node if the parent row is associated with a top level item that can have children.
    if(parent.isValid()) {
        switch(parent.row()) {
            case TopLevelItem::MATERIALS: return createIndex(row, column, ItemType::MATERIAL);
            case TopLevelItem::LAYERS: return createIndex(row, column, ItemType::LAYER);
            case TopLevelItem::PROFILE: return createIndex(row, column, ItemType::SEGMENT);
        }
    }

    return QModelIndex();
}

QModelIndex MainTreeModel::parent(const QModelIndex &index) const {
    // Invalid index is the root item and has no parent
    if(!index.isValid()) {
        return QModelIndex();
    }

    // Top-level nodes have invalid root node as parent
    if(index.internalId() == ItemType::TOPLEVEL) {
        return QModelIndex();
    }

    // Other nodes have their associated top-level node as parent
    switch(index.internalId()) {
        case ItemType::MATERIAL: return createIndex(TopLevelItem::MATERIALS, 0, ItemType::TOPLEVEL);
        case ItemType::LAYER: return createIndex(TopLevelItem::LAYERS, 0, ItemType::TOPLEVEL);
        case ItemType::SEGMENT: return createIndex(TopLevelItem::PROFILE, 0, ItemType::TOPLEVEL);
    }

    return QModelIndex();  // Should not happen
}

int MainTreeModel::rowCount(const QModelIndex &parent) const {
    // No rows if no bow model is assigned
    if(bow == nullptr) {
        return 0;
    }

    // If the parent is invalid, we are at root which has as many rows as there are top-level items
    if(!parent.isValid()) {
        return TopLevelItem::COUNT;
    }

    // Otherwise check if the parent is a top-level item and determine the number of child nodes accordingly
    if(parent.internalId() == ItemType::TOPLEVEL) {
        switch(parent.row()) {
            case TopLevelItem::MATERIALS: return bow->section.materials.size();
            case TopLevelItem::LAYERS: return bow->section.layers.size();
            case TopLevelItem::PROFILE: return bow->profile.segments.size();
        }
    }

    return 0;    // Everything else has no children
}

int MainTreeModel::columnCount(const QModelIndex &parent) const {
    return 1;
}


Qt::ItemFlags MainTreeModel::flags(const QModelIndex &index) const {
    // Get default flags of the parent implementation
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);

    // In case of materials and layers, add the editable flag
    if(index.internalId() == ItemType::MATERIAL || index.internalId() == ItemType::LAYER) {
        flags |= Qt::ItemIsEditable;
    }

    return flags;
}

QVariant MainTreeModel::data(const QModelIndex &index, int role) const {
    // No data if no bow model is assigned
    if(bow == nullptr) {
        return QVariant();
    }

    // If the parent is invalid, we are at the top level
    if(!index.parent().isValid()) {
        switch(role) {
            case Qt::DisplayRole: return topLevelItemName(index.row());
            case Qt::ToolTipRole: return topLevelToolTip(index.row());
            case Qt::DecorationRole: return topLevelItemIcon(index.row());
            default: return QVariant();
        }
    }

    if(index.parent().row() == TopLevelItem::MATERIALS) {
        auto& material = *std::next(bow->section.materials.begin(), index.row());
        switch(role) {
            case Qt::DisplayRole: case Qt::EditRole: return QString::fromStdString(material.name);
            case Qt::ToolTipRole: return "User-defined material \"" + QString::fromStdString(material.name) + "\"";
            case Qt::DecorationRole: return QIcon(":/icons/model-material.svg");
            default: return QVariant();
        }
    }

    if(index.parent().row() == TopLevelItem::LAYERS) {
        auto& layer = *std::next(bow->section.layers.begin(), index.row());
        switch(role) {
            case Qt::DisplayRole: case Qt::EditRole: return QString::fromStdString(layer.name);
            case Qt::ToolTipRole: return "User-defined layer \"" + QString::fromStdString(layer.name) + "\"";
            case Qt::DecorationRole: return QIcon(":/icons/model-layer.svg");
            default: return QVariant();
        }
    }

    if(index.parent().row() == TopLevelItem::PROFILE) {
        switch(role) {
            case Qt::DisplayRole: return QString::number(index.row()) + ": " + segmentName(index.row());
            case Qt::ToolTipRole: return segmentTooltip(index.row());
            case Qt::DecorationRole: return segmentIcon(index.row());
            default: return QVariant();
        }
    }

    return QVariant();
}

bool MainTreeModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if(role != Qt::EditRole) {
        return false;
    }

    // Set material name
    if(index.internalId() == ItemType::MATERIAL) {
        std::string newName = value.toString().toStdString();
        if(!bow->isValidMaterialName(newName)) {
            return false;
        }

        // Rename material
        Material& material = *std::next(bow->section.materials.begin(), index.row());
        std::string oldName = material.name;
        material.name = newName;

        // Change the name also in layers that use the material
        for(auto& layer: bow->section.layers) {
            if(layer.material == oldName) {
                layer.material = newName;
            }
        }

        emit dataChanged(index, index);
        return true;
    }

    // Set layer name
    if(index.internalId() == ItemType::LAYER) {
        std::string name = value.toString().toStdString();
        if(!bow->isValidLayerName(name)) {
            return false;
        }

        std::next(bow->section.layers.begin(), index.row())->name = name;
        emit dataChanged(index, index);
        return true;
    }

    return false;
}

QString MainTreeModel::topLevelItemName(int row) const {
    switch(row) {
        case TopLevelItem::COMMENTS: return "Comments";
        case TopLevelItem::SETTINGS: return "Settings";
        case TopLevelItem::DIMENSIONS: return "Dimensions";
        case TopLevelItem::MATERIALS: return "Materials";
        case TopLevelItem::LAYERS: return "Layers";
        case TopLevelItem::PROFILE: return "Profile";
        case TopLevelItem::WIDTH: return "Width";
        case TopLevelItem::STRING: return "String";
        case TopLevelItem::MASSES: return "Masses";
        case TopLevelItem::DAMPING: return "Damping";
        default: throw std::invalid_argument("Unknown enum variant");
    }
}

QString MainTreeModel::topLevelToolTip(int row) const {
    switch(row) {
        case TopLevelItem::COMMENTS: return "Comments about this bow";
        case TopLevelItem::SETTINGS: return "Settings for the simulation";
        case TopLevelItem::DIMENSIONS: return "Brace height, draw length and handle geometry";
        case TopLevelItem::MATERIALS: return "Materials that can be assigned to the layers";
        case TopLevelItem::LAYERS: return "Layers that make up the bow limbs";
        case TopLevelItem::PROFILE: return "Initial profile shape of the bow";
        case TopLevelItem::WIDTH: return "Width variation of the limbs";
        case TopLevelItem::STRING: return "Properties of the bowstring";
        case TopLevelItem::MASSES: return "Masses of the arrow and other components";
        case TopLevelItem::DAMPING: return "Damping properties of limbs and string";
        default: throw std::invalid_argument("Unknown enum variant");
    }
}

QIcon MainTreeModel::topLevelItemIcon(int row) const {
    switch(row) {
        case TopLevelItem::COMMENTS: return QIcon(":/icons/model-comments.svg");
        case TopLevelItem::SETTINGS: return QIcon(":/icons/model-settings.svg");
        case TopLevelItem::DIMENSIONS: return QIcon(":/icons/model-dimensions.svg");
        case TopLevelItem::MATERIALS: return QIcon(":/icons/model-materials.svg");
        case TopLevelItem::LAYERS: return QIcon(":/icons/model-layers.svg");
        case TopLevelItem::PROFILE: return QIcon(":/icons/model-profile.svg");
        case TopLevelItem::WIDTH: return QIcon(":/icons/model-width.svg");
        case TopLevelItem::STRING: return QIcon(":/icons/model-string.svg");
        case TopLevelItem::MASSES: return QIcon(":/icons/model-masses.svg");
        case TopLevelItem::DAMPING: return QIcon(":/icons/model-damping.svg");
        default: throw std::invalid_argument("Unknown enum variant");
    }
}

QString MainTreeModel::segmentName(int row) const {
    ProfileSegment& segment = *std::next(bow->profile.segments.begin(), row);

    if(std::holds_alternative<Line>(segment)) {
        return "Line";
    }
    if(std::holds_alternative<Arc>(segment)) {
        return "Arc";
    }
    if(std::holds_alternative<Spiral>(segment)) {
        return "Spiral";
    }
    if(std::holds_alternative<Spline>(segment)) {
        return "Spline";
    }

    throw std::invalid_argument("Unknown segment type");
}

QString MainTreeModel::segmentTooltip(int row) const {
    ProfileSegment& segment = *std::next(bow->profile.segments.begin(), row);

    if(std::holds_alternative<Line>(segment)) {
        return "Line segment defined by a single length";
    }
    if(std::holds_alternative<Arc>(segment)) {
        return "Arc segment defined by length and radius";
    }
    if(std::holds_alternative<Spiral>(segment)) {
        return "Spiral segment defined by length, start-radius and end-radius";
    }

    if(std::holds_alternative<Spline>(segment)) {
        return "Spline segment defined by a series of control points";
    }

    throw std::invalid_argument("Unknown segment type");
}

QIcon MainTreeModel::segmentIcon(int row) const {
    ProfileSegment& segment = *std::next(bow->profile.segments.begin(), row);

    if(std::holds_alternative<Line>(segment)) {
        return QIcon(":/icons/segment-line.svg");
    }
    if(std::holds_alternative<Arc>(segment)) {
        return QIcon(":/icons/segment-arc.svg");
    }
    if(std::holds_alternative<Spiral>(segment)) {
        return QIcon(":/icons/segment-spiral.svg");
    }
    if(std::holds_alternative<Spline>(segment)) {
        return QIcon(":/icons/segment-spline.svg");
    }

    throw std::invalid_argument("Unknown segment type");
}


