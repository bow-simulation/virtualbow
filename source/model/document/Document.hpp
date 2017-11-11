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


/*
#include "numerics/Domain.hpp"

#include <boost/signals2.hpp>
#include <functional>
#include <QtCore>

class DocumentNode: public QObject
{
    Q_OBJECT

public:
    DocumentNode(): modified(false)
    {

    }

signals:
    void stateChanged(bool modified);

private:
    bool modified;
};


typedef boost::signals2::scoped_connection Connection;

template<typename T>
class DocumentItem
{
public:
    typedef boost::signals2::signal<void(const T&)> Signal;
    typedef std::function<bool(const T&)> Validator;
    Validator validate;

    DocumentItem(DocumentNode* document, Validator validate = [](const T&){ return true; })
        : document(document),
          validate(validate)
    {

    }

    operator const T&() const
    {
        return value;
    }

    DocumentItem& operator=(const T& rhs)
    {
        if(validate(rhs))
        {
            if(rhs != value)
            {
                value = rhs;
                //document->setModified(true);
                signal(value);
            }
        }
        else
        {
            throw std::runtime_error("Invalid value");
        }

        return *this;
    }

    Connection connect(const typename Signal::slot_type& slot)
    {
        slot(value);
        return signal.connect(slot);
    }

private:
    T value;
    DocumentNode* document;
    Signal signal;
};
*/
