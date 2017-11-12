#pragma once
#include <QtCore>
#include "Validators.hpp"
#include <boost/optional.hpp>

class DocumentNode: public QObject
{
    Q_OBJECT

public:
    DocumentNode(DocumentNode& parent)
    {
        QObject::connect(this, &DocumentNode::modified, &parent, &DocumentNode::modified);    // Propagate modifications upwards
    }

    DocumentNode() = default;
    virtual ~DocumentNode() = default;

signals:
    void modified();
};

template<typename T>
class DocumentItem: public DocumentNode
{
public:
    using Validator = std::function<boost::optional<std::string>(const T&)>;

    /*
    friend void to_json(json&, const DocumentItem<T>&);
    friend void from_json(const json&, DocumentItem<T>&);
    */

    DocumentItem(DocumentNode& parent, const Validator& validator, const T& value)
        : DocumentNode(parent),
          current_value(value),
          validator(validator)
    {
        emit modified();
    }

    operator const T&() const
    {
        return current_value;
    }

    DocumentItem& operator=(const T& rhs)
    {
        current_value = rhs;
        emit modified();

        return *this;
    }

private:
    Validator validator;
    T current_value;
};
