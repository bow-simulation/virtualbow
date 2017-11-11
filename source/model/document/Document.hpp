#pragma once
#include "Validators.hpp"
#include <boost/signals2.hpp>
#include <boost/optional.hpp>
#include <string>
#include <set>

class DocumentNode
{
public:
    DocumentNode() = default;

    DocumentNode(DocumentNode& parent)
        : parent(&parent)
    {
        this->parent->children.insert(this);
        this->parent->sig_value_changed();

        sig_value_changed.connect([&]()
        {
            // Search children for errors, use the first one found
            for(auto child: children)
            {
                if(child->error_state())
                    error = child->error_state();
            }

            // Children have no errors, call own validation function
            error = validate();

            // Propagate change upwards
            this->parent->sig_value_changed();
        });
    }

    virtual ~DocumentNode()
    {
        if(parent)
        {
            this->parent->children.erase(this);
            this->parent->sig_value_changed();
        }
    }

    template<class F>
    void on_value_changed(const F& f)
    {
        sig_value_changed.connect(f);
    }

    virtual boost::optional<std::string> validate()
    {
        return boost::none;
    }

    const boost::optional<std::string>& error_state() const
    {
        return error;
    }

protected:
    // Tree stuff
    DocumentNode* parent = nullptr;
    std::set<DocumentNode*> children;

    // Error handling
    boost::optional<std::string> error = boost::none;

    // Signals
    boost::signals2::signal<void()> sig_value_changed;
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
        sig_value_changed();
    }

    operator const T&() const
    {
        return current_value;
    }

    DocumentItem& operator=(const T& rhs)
    {
        current_value = rhs;
        sig_value_changed();

        return *this;
    }

    virtual boost::optional<std::string> validate()
    {
        return validator(current_value);
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
