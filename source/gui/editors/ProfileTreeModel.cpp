#include "ProfileTreeModel.hpp"
#include <algorithm>

ProfileTreeModel::ProfileTreeModel(QObject *parent)
    : QAbstractTableModel(parent)
{

}

// Insert new segment below the last selected row or at the end of the list if the selection is empty
void ProfileTreeModel::insertSegment(const QModelIndexList& selection, const SegmentInput& segment) {
    auto it = std::max_element(selection.begin(), selection.end());         // Last selected model index
    int row = (it != selection.end()) ? (*it).row() + 1 : segments.size();  // Insert at last selected index, if it exists, or at the end

    beginInsertRows(QModelIndex(), row, row);
    segments.insert(segments.begin() + row, segment);
    endInsertRows();
}

// Remove selected segments or the last segment if the selection is empty
void ProfileTreeModel::removeSegments(const QModelIndexList& selection) {
    if(selection.isEmpty()) {
        if(!segments.empty()) {
            int row = segments.size() - 1;
            beginRemoveRows(QModelIndex(), row, row);
            segments.erase(segments.begin() + row);
            endRemoveRows();
        }
    }
    else {
        for(auto& index: selection) {
            int row = index.row();
            beginRemoveRows(QModelIndex(), row, row);
            segments.erase(segments.begin() + row);
            endRemoveRows();
        }
    }
}

Qt::DropActions ProfileTreeModel::supportedDropActions() const {
    return Qt::MoveAction;
}

Qt::ItemFlags ProfileTreeModel::flags(const QModelIndex &index) const {
    Qt::ItemFlags default_flags = QAbstractTableModel::flags(index);
    if (index.isValid()) {
        return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | default_flags;
    }
    else {
        return Qt::ItemIsDropEnabled | default_flags;
    }
}

bool ProfileTreeModel::removeRows(int row, int count, const QModelIndex &parent) {
    qInfo() << "Remove Rows";
    segments.erase(segments.begin() + row, segments.begin() + row + count);
    return true;
}

int ProfileTreeModel::rowCount(const QModelIndex& parent) const {
   return segments.size();
}

int ProfileTreeModel::columnCount(const QModelIndex& parent) const {
    return 1;
}

QVariant ProfileTreeModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        if(section == 0) {
            return QString("Segments");
        }
    }
    return QVariant();
}

QVariant ProfileTreeModel::data(const QModelIndex& index, int role) const {
    auto& segment = segments[index.row()];

    switch(role) {
        case Qt::DisplayRole:
            return ("%1 - " + segmentText(segment.type)).arg(index.row() + 1);

        case Qt::DecorationRole:
            return segmentIcon(segment.type);
    }

    return QVariant();
}

QIcon ProfileTreeModel::segmentIcon(SegmentType type) const {
    switch(type) {
        case SegmentType::Line:
            return QIcon(":/icons/segment-line.svg");

        case SegmentType::Arc:
            return QIcon(":/icons/segment-arc.svg");

        case SegmentType::Spiral:
            return QIcon(":/icons/segment-spiral.svg");

        case SegmentType::Spline:
            return QIcon(":/icons/segment-spline.svg");
    }

    return QIcon();
}

QString ProfileTreeModel::segmentText(SegmentType type) const {
    switch(type) {
        case SegmentType::Line:
            return "Line";

        case SegmentType::Arc:
            return "Arc";

        case SegmentType::Spiral:
            return "Spiral";

        case SegmentType::Spline:
            return "Spline";
    }

    return QString();
}