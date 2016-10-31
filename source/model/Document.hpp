#pragma once;
#include "../numerics/Domain.hpp"
#include <boost/signals2.hpp>
#include <cereal/cereal.hpp>
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
    Validator validate;

    DocItem(T value, Document* document, Validator validate = [](const T&){ return true; })
        : value(value),
          document(document),
          validate(validate)
    {
        if(!validate(value))
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
        if(validate(rhs))
        {
            value = rhs;
            document->set_modified(true);
        }

        signal(value);

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
    }

private:
    T value;
    Document* document;
    Signal signal;
};
