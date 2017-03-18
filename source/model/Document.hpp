#pragma once
#include "numerics/Domain.hpp"

#include <boost/signals2.hpp>
#include <functional>
#include <QtCore>

class Document: public QObject
{
    Q_OBJECT

public:
    Document(): modified(false)
    {

    }

    bool isModified() const
    {
        return modified;
    }

    void setModified(bool value)
    {
        modified = value;
        emit stateChanged(modified);
    }

signals:
    void stateChanged(bool modified);

private:
    bool modified;
};


typedef boost::signals2::scoped_connection Connection;

template<typename T>
class DocItem
{
public:
    typedef boost::signals2::signal<void(const T&)> Signal;
    typedef std::function<bool(const T&)> Validator;
    Validator validate;

    DocItem(Document* document, Validator validate = [](const T&){ return true; })
        : document(document),
          validate(validate)
    {

    }

    operator const T&() const
    {
        return value;
    }

    DocItem& operator=(const T& rhs)
    {
        if(validate(rhs))
        {
            if(rhs != value)
            {
                value = rhs;
                document->setModified(true);
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

    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(value);

        if(!validate(value))
        {
            throw std::runtime_error("Invalid value");
        }
    }

private:
    T value;
    Document* document;
    Signal signal;
};
