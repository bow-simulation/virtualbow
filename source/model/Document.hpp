#pragma once
#include "../numerics/Domain.hpp"
#include <boost/signals2.hpp>
#include <functional>

class Document
{
public:
    Document(): modified(false)
    {

    }

    bool is_modified() const
    {
        return modified;
    }

    void set_modified(bool m)
    {
        modified = m;
    }

private:
    bool modified;
};

template<typename T>
class DocItem
{
public:
    typedef boost::signals2::signal<void(const T&)> Signal;
    typedef boost::signals2::scoped_connection Connection;
    typedef std::function<bool(const T&)> Validator;

    DocItem(T value, Document* document, Validator validator = [](const T&){ return true; })
        : value(value),
          document(document),
          validator(validator)
    {
        if(!validator(value))
        {
            throw std::runtime_error("Invalid initial value");
        }
    }

    operator const T&() const
    {
        return value;
    }

    DocItem& operator=(const T& rhs)
    {
        if(validator(rhs))
        {
            value = rhs;
            document->set_modified(true);
            signal(value);
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

        if(!validator(value))
        {
            throw std::runtime_error("Invalid value");
        }
    }

private:
    T value;
    Document* document;
    Validator validator;
    Signal signal;
};
