#include "ProfileTreeModel.hpp"
#include <QStandardItem>
#include <algorithm>

int ProfileTreeModel::rowCount(const QModelIndex& parent) const {
    return input.size();
}

QVariant ProfileTreeModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if(role == Qt::DisplayRole && section == 0 && orientation == Qt::Horizontal) {
        return "Segments";
    }

    return QVariant();
}

QVariant ProfileTreeModel::data(const QModelIndex& index, int role) const {
    if(index.isValid() && index.row() < input.size()) {
        if(role == Qt::DisplayRole) {
            return segmentText(input[index.row()]);
        }
        if(role == Qt::DecorationRole) {
            return segmentIcon(input[index.row()]);
        }
    }

    return QVariant();
}

void ProfileTreeModel::appendSegment(const SegmentInput& segment) {
    beginInsertRows(QModelIndex(), input.size(), input.size());
    input.push_back(segment);
    endInsertRows();

    emit modified();
}

void ProfileTreeModel::insertSegment(const QModelIndex& index, const SegmentInput& segment) {
    beginInsertRows(QModelIndex(), index.row(), index.row());
    input.insert(input.begin() + index.row(), segment);
    endInsertRows();

    emit modified();
}

void ProfileTreeModel::removeSegments(const QModelIndexList& indexes) {
    for(QModelIndex index: indexes) {
        beginRemoveRows(QModelIndex(), index.row(), index.row());
        input.erase(input.begin() + index.row());
        endRemoveRows();
    }

    emit modified();
}

void ProfileTreeModel::removeSegment() {
    if(!input.empty()) {
        int target = input.size() - 1;
        beginRemoveRows(QModelIndex(), target, target);
        input.erase(input.begin() + target);
        endRemoveRows();
    }

    emit modified();
}

void ProfileTreeModel::moveSegmentsUp(QModelIndexList indexes) {
    std::sort(indexes.begin(), indexes.end(), qLess<QModelIndex>());  // TODO: use std::less?
    for(QModelIndex index: indexes) {
        int source = index.row();
        int target = index.row() - 1;
        if(target < 0 || target >= input.size()) {
            return;
        }

        beginMoveRows(QModelIndex(), source, source, QModelIndex(), target);
        std::swap(input[source], input[target]);
        endMoveRows();
    }

    emit modified();
}

void ProfileTreeModel::moveSegmentsDown(QModelIndexList indexes) {
    std::sort(indexes.begin(), indexes.end(), qGreater<QModelIndex>());  // TODO: use std::greater?
    for(QModelIndex index: indexes) {
        int source = index.row();
        int target = index.row() + 1;
        if(target < 0 || target >= input.size()) {
            return;
        }

        beginMoveRows(QModelIndex(), target, target, QModelIndex(), source);
        std::swap(input[source], input[target]);
        endMoveRows();
    }

    emit modified();
}

const SegmentInput& ProfileTreeModel::getSegment(const QModelIndex& index) const {
    if(index.isValid() && index.row() < input.size()) {
        return input[index.row()];
    }
}

void ProfileTreeModel::setSegment(const QModelIndex& index, const SegmentInput& segment) {
    if(index.isValid() && index.row() < input.size()) {
        input[index.row()] = segment;
        emit dataChanged(index, index);
        emit modified();
    }
}

const ProfileInput& ProfileTreeModel::getData() const {
    return input;
}

void ProfileTreeModel::setData(const ProfileInput& data) {
    size_t old_size = input.size();
    size_t new_size = data.size();

    if(new_size > old_size) {
        // Rows are being added
        beginInsertRows(QModelIndex(), old_size - 1, new_size - 1);
        input = data;
        endInsertRows();

    }
    else if(old_size < new_size) {
        // Rows are being removed
        beginRemoveRows(QModelIndex(), new_size - 1, old_size - 1);
        input = data;
        endRemoveRows();
    }
    else {
        // No change to number of rows
        input = data;
    }

    // Any data might have changed
    emit dataChanged(index(0), index(new_size - 1));
}

QIcon ProfileTreeModel::segmentIcon(const SegmentInput& segment) const {
    if(std::holds_alternative<LineInput>(segment)) {
        return QIcon(":/icons/segment-line.svg");
    }
    if(std::holds_alternative<ArcInput>(segment)) {
        return QIcon(":/icons/segment-arc.svg");
    }
    if(std::holds_alternative<SpiralInput>(segment)) {
        return QIcon(":/icons/segment-spiral.svg");
    }
    if(std::holds_alternative<SplineInput>(segment)) {
        return QIcon(":/icons/segment-spline.svg");
    }

    throw std::invalid_argument("Unknown segment type");
}

QString ProfileTreeModel::segmentText(const SegmentInput& segment) const {
    if(std::holds_alternative<LineInput>(segment)) {
        return "Line";
    }
    if(std::holds_alternative<ArcInput>(segment)) {
        return "Arc";
    }
    if(std::holds_alternative<SpiralInput>(segment)) {
        return "Spiral";
    }
    if(std::holds_alternative<SplineInput>(segment)) {
        return "Spline";
    }

    throw std::invalid_argument("Unknown segment type");
}

