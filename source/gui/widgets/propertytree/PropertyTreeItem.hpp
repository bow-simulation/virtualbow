#pragma once
#include <QList>
#include <QVariant>

class PropertyTreeItem
{
public:
    PropertyTreeItem(const QString& name = QString(), const QVariant& value = QVariant(), PropertyTreeItem* parent = nullptr);
    ~PropertyTreeItem();

    const QString& getName() const;
    const QVariant& getValue() const;

    PropertyTreeItem* getParent();
    const QList<PropertyTreeItem*> getChildren() const;

private:
    PropertyTreeItem* parent;
    QList<PropertyTreeItem*> children;

    QString name;
    QVariant value;
};
