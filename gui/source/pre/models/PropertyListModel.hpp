#pragma once
#include "solver/BowModel.hpp"
#include <QVariant>
#include <QColor>
#include <QAbstractListModel>

class AbstractProperty: public QObject {
public:
    virtual QVariant data(int role) const = 0;
    virtual bool setData(const QVariant &value, int role) = 0;
};

class StringProperty: public AbstractProperty {
public:
    StringProperty(QObject* parent, std::string& value): value(value) {
        setParent(parent);  // TODO: Do this by calling super-constructor?
    }

    virtual QVariant data(int role) const override {
        if(role == Qt::DisplayRole || role == Qt::EditRole ) {
            return QString::fromStdString(value);
        }

        return QVariant();
    }

    virtual bool setData(const QVariant &value, int role) override {
        if(role != Qt::EditRole) {
            return false;
        }

        this->value = value.toString().toStdString();
        return true;
    }

private:
    std::string& value;
};

class ColorProperty: public AbstractProperty {
public:
    ColorProperty(QObject* parent, std::string& value): value(value) {
        setParent(parent);  // TODO: Do this by calling super-constructor?
    }

    virtual QVariant data(int role) const override {
        if(role == Qt::DisplayRole || role == Qt::EditRole ) {
            return QColor(QString::fromStdString(value));
        }

        return QVariant();
    }

    virtual bool setData(const QVariant &value, int role) override {
        if(role != Qt::EditRole) {
            return false;
        }

        this->value = value.toString().toStdString();
        return true;
    }

private:
    std::string& value;
};


class IntegerProperty: public AbstractProperty {
public:
    IntegerProperty(QObject* parent, int& value):
        value(value)
    {
        setParent(parent);  // TODO: Do this by calling super-constructor?
    }

    virtual QVariant data(int role) const override {
        if(role == Qt::DisplayRole || role == Qt::EditRole ) {
            return value;
        }

        return QVariant();
    }

    virtual bool setData(const QVariant &value, int role) override {
        if(role != Qt::EditRole) {
            return false;
        }

        bool success;
        int parsed = value.toInt(&success);
        if(!success) {
            return false;
        }

        this->value = parsed;
        return true;
    }

private:
    int& value;
};

class DoubleProperty: public AbstractProperty {
public:
    DoubleProperty(QObject* parent, double& value):
        value(value)
    {
        setParent(parent);  // TODO: Do this by calling super-constructor?
    }

    virtual QVariant data(int role) const override {
        if(role == Qt::DisplayRole || role == Qt::EditRole ) {
            return value;
        }

        return QVariant();
    }

    virtual bool setData(const QVariant &value, int role) override {
        if(role != Qt::EditRole) {
            return false;
        }

        bool success;
        double parsed = value.toDouble(&success);
        if(!success) {
            return false;
        }

        this->value = parsed;
        return true;
    }

private:
    double& value;
};

template<typename EnumType>
class EnumProperty: public AbstractProperty {
public:
    EnumProperty(QObject* parent, EnumType& value):
        value(value)
    {
        setParent(parent);  // TODO: Do this by calling super-constructor?
    }

    virtual QVariant data(int role) const override {
        if(role == Qt::DisplayRole || role == Qt::EditRole ) {
            return static_cast<int>(value);
        }

        return QVariant();
    }

    virtual bool setData(const QVariant &value, int role) override {
        if(role != Qt::EditRole) {
            return false;
        }

        bool success;
        int parsed = value.toInt(&success);
        if(!success) {
            return false;
        }

        this->value = static_cast<EnumType>(parsed);
        return true;
    }

private:
    EnumType& value;
};

template<typename CustomType>
class CustomTypeProperty: public AbstractProperty {
public:
    CustomTypeProperty(QObject* parent, CustomType& value):
        value(value)
    {
        setParent(parent);  // TODO: Do this by calling super-constructor?
    }

    virtual QVariant data(int role) const override {
        if(role == Qt::DisplayRole || role == Qt::EditRole ) {
            return QVariant::fromValue(value);
        }

        return QVariant();
    }

    virtual bool setData(const QVariant &value, int role) override {
        if(role != Qt::EditRole) {
            return false;
        }

        if(!value.canConvert<CustomType>()) {
            return false;
        }

        this->value = value.value<CustomType>();
        return true;
    }

private:
    CustomType& value;
};

class PropertyListModel: public QAbstractListModel {
    Q_OBJECT

public:
    PropertyListModel();

    QPersistentModelIndex addString(std::string& value);
    QPersistentModelIndex addColor(std::string& value);
    QPersistentModelIndex addInteger(int& value);
    QPersistentModelIndex addDouble(double& value);

    template<typename EnumType>
    QPersistentModelIndex addEnum(EnumType& value) {
        return addProperty(new EnumProperty<EnumType>(this, value));
    }

    template<typename CustomType>
    QPersistentModelIndex addCustom(CustomType& value) {
        return addProperty(new CustomTypeProperty(this, value));
    }

    // Implementation of abstract methods

    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

signals:
    void contentModified();

private:
    QList<AbstractProperty*> properties;

    QPersistentModelIndex addProperty(AbstractProperty* property);
};
