#pragma once;
#include <boost/signals2.hpp>
#include <cereal/cereal.hpp>

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

    DocItem(Document* doc, T val)
        : doc(doc),
          val(val)
    {

    }

    operator const T&() const
    {
        return val;
    }

    DocItem& operator=(const T& rhs)
    {
        val = rhs;
        doc->set_modified(true);
        sig(val);

        return *this;
    }

    Connection connect(const typename Signal::slot_type& slot)
    {
        slot(val);
        return sig.connect(slot);
    }

    template<class Archive>
    void serialize(Archive & archive)
    {
        archive(val);
    }

private:
    Document* doc;
    Signal sig;
    T val;
};
