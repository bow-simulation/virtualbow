#pragma once
#include "pre/viewmodels/MainVM.hpp"
#include "solver/BowModel.hpp"
#include "solver/Defaults.hpp"
#include <QAbstractItemModel>
#include <QStringList>

// Types of items in the model tree, exposed via the id's of the model indices
enum ItemType {
    TOPLEVEL,    // Top level items, which can have other item types as children
    MATERIAL,    // Single material item under "Materials"
    LAYER,       // Single layer item under "Layers"
    SEGMENT      // Single profile segment under "Profile"
};

// The top-level items in the model tree and their row indices
enum TopLevelItem {
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

    LAST = DAMPING,
    COUNT = LAST + 1
};

class ModelTreeVM: public QAbstractItemModel {
    Q_OBJECT

public:
    ModelTreeVM(MainVM* parent);
    void setBowModel(BowModel* bow);

    bool canInsertMaterial(const QModelIndexList& indexes);
    void insertMaterial(int row);
    void appendMaterial();

    bool canInsertLayer(const QModelIndexList& indexes);
    void insertLayer(int row);
    void appendLayer();

    bool canInsertSegment(const QModelIndexList& indexes);
    void insertSegment(int row, SegmentType type);
    void appendSegment(SegmentType type);

    bool canRemoveIndexes(QModelIndexList& indexes);
    void removeIndexes(QModelIndexList indexes);
    void removeMaterial(int row);
    void removeLayer(int row);
    void removeSegment(int row);

    bool canMoveIndexesUp(const QModelIndexList& indexes);
    void moveIndexesUp(QModelIndexList indexes);

    bool canMoveIndexesDown(const QModelIndexList& indexes);
    void moveIndexesDown(QModelIndexList indexes);

    void swapMaterials(int i, int j);
    void swapLayers(int i, int j);
    void swapSegments(int i, int j);

    // Abstract method implementations

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;

    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

signals:
    // Emitted when the tree structure of the bow model has been modified (including names)
    void contentModified();

private:
    BowModel* bow;

    QString topLevelItemName(int row) const;
    QString topLevelToolTip(int row) const;
    QIcon topLevelItemIcon(int row) const;

    QString segmentName(int row) const;
    QString segmentTooltip(int row) const;
    QIcon segmentIcon(int row) const;
};
