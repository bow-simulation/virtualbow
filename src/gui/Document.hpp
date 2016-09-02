#pragma once
#include "../model/InputData.hpp"
#include <functional>
#include <boost/signals2.hpp>

class Document
{
public:
    template<typename T>
    friend class DocumentItem;

    Document(): modified(false)
    {

    }

    InputData getData() const
    {
        return data;
    }

    void setData(InputData val)
    {
        data = val;

        modified = false;
        update();
    }

    bool isModified()
    {
        return modified;
    }

private:
    InputData data;                 // Todo: Make InputData a template parameter
    bool modified;
    boost::signals2::signal<void()> update;
};

template<typename T>
class DocumentItem
{
public:
    using ItemFunction = std::function<T&(InputData&)>; // Todo: using or typedef? Replace typedefs with using?
    using UpdateFunction = std::function<void()>;

    DocumentItem(Document& doc, ItemFunction fn)
        : document(doc),
          item_fn(fn),
          update_fn([](){})
    {
        connection = document.update.connect(update_fn);
    }

    ~DocumentItem()
    {
        connection.disconnect();
    }

    void connect(UpdateFunction fn) // Todo: Different name? Can only connect one function at a time.
    {
        update_fn = fn;
        update_fn();
    }

    T getData() const
    {
        return item_fn(document.data);
    }

    void setData(T val)
    {
        item_fn(document.data) = val;

        document.modified = true;
        document.update();
    }

private:
    Document& document;
    ItemFunction item_fn;
    UpdateFunction update_fn;

    boost::signals2::connection  connection;
};
