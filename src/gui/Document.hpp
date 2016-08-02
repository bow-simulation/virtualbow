#pragma once
#include <set>
#include <functional>
#include <algorithm>

#include "../model/BowParameters.hpp"

#include <QtCore>

class ViewBase;

template<typename T>
class View;

class Document
{
public:
    template<typename T>
    friend class View;

    Document();
    void load(const std::string& path);
    void save(const std::string& path);
    const BowParameters& getData();

    void addView(ViewBase* view);
    void removeView(ViewBase* view);
    void updateViews();

private:
    BowParameters data;
    std::set<ViewBase*> views;
    bool modified;
};

class ViewBase
{
public:
    virtual ~ViewBase(){}
    virtual void updateDoc()= 0;
    virtual void updateView() = 0;
};

template<typename T>
using ViewFunction = std::function<T&(BowParameters&)>;

template<typename T>
class View: public ViewBase
{
public:
    View(Document& doc, ViewFunction<T> view_fn)
        : doc(doc), view_fn(view_fn)
    {
        doc.addView(this);
    }

    virtual ~View()
    {
        doc.removeView(this);
    }

    virtual void updateDoc() override
    {
        updateDoc(view_fn(doc.data));
        doc.updateViews();
        doc.modified = true;
    }

    virtual void updateView() override
    {
        updateView(view_fn(doc.data));
    }

    virtual void updateDoc(T& data) = 0;
    virtual void updateView(const T& data) = 0;

private:
    ViewFunction<T> view_fn;
    Document& doc;
};
