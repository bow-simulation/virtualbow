#pragma once
#include <QtCore>
#include "Validators.hpp"
#include <boost/optional.hpp>
#include <boost/range/algorithm_ext/erase.hpp>
#include <boost/range/algorithm.hpp>
#include <string>
#include <json.hpp>

using nlohmann::json;

class DocumentNode: public QObject
{
    Q_OBJECT

public:
    DocumentNode(DocumentNode& parent)
        : parent(parent)
    {
        QObject::connect(this, &DocumentNode::value_changed, &parent, &DocumentNode::value_changed);    // Propagate modifications upwards
    }

    DocumentNode() = default;
    virtual ~DocumentNode() = default;

    const std::vector<std::string>& get_errors() const
    {
        return errors;
    }

    void add_error(const std::string& e)
    {
        if(boost::count(errors, e) == 0)
        {
            errors.push_back(e);
            emit error_changed();
            qInfo() << "Emit error changed, e = " << QString::fromStdString(e);
        }

        if(parent)
        {
            parent->add_error(e);
        }
    }

    void remove_error(const std::string& e)
    {
        if(boost::count(errors, e) != 0)
        {
            boost::remove_erase(errors, e);
            emit error_changed();
        }

        if(parent)
        {
            parent->remove_error(e);
        }
    }

signals:
    void value_changed();
    void error_changed();

private:
    boost::optional<DocumentNode&> parent;
    std::vector<std::string> errors;
};

template<typename T>
class DocumentItem: public DocumentNode
{
public:
    DocumentItem(DocumentNode& parent, const T& value)
        : DocumentNode(parent),
          current_value(value)
    {
        emit value_changed();
    }

    operator const T&() const
    {
        return current_value;
    }

    DocumentItem& operator=(const T& rhs)
    {
        current_value = rhs;
        emit value_changed();

        return *this;
    }

private:
    T current_value;
};

template<typename T>
static void to_json(json& obj, const DocumentItem<T>& value)
{
    obj = T(value);
}

template<typename T>
static void from_json(const json& obj, DocumentItem<T>& value)
{
    value = obj.get<T>();
}
