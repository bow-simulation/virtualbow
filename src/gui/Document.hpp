#pragma once
#include "../model/InputData.hpp"
#include <functional>
#include <QtCore>

class Document: public QObject
{
    Q_OBJECT

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
        emit update();
    }

    bool isModified()
    {
        return modified;
    }

signals:
    void update();

private:
    InputData data;
    bool modified;
};

template<typename T>
class DocumentItem
{
public:
    using ItemFunction = std::function<T&(InputData&)>;     // Todo: using or typedef? Replace typedefs with using?

    DocumentItem(Document& doc, ItemFunction fn)
        : document(doc),
          item_fn(fn)
    {

    }

    ~DocumentItem()
    {

    }

    template<typename Function>
    void connect(Function fn)
    {
        QObject::connect(&document, &Document::update, fn);
        fn();
    }

    T getData() const
    {
        return item_fn(document.data);
    }

    void setData(T val)
    {
        item_fn(document.data) = val;

        document.modified = true;
        emit document.update();
    }

private:
    Document& document;
    ItemFunction item_fn;
};
