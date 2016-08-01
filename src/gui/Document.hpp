#pragma once
#include <set>
#include <functional>
#include <algorithm>

#include "../model/BowParameters.hpp"

#include <QtCore>

class ViewBase;

class Document
{
public:
    BowParameters& getData();

    void addView(ViewBase* view);
    void removeView(ViewBase* view);
    void updateViews();

private:
    BowParameters data;
    std::set<ViewBase*> views;
};

class ViewBase
{
public:
    ViewBase(): document(nullptr)
    {

    }

    ~ViewBase()
    {
        if(document != nullptr)
            document->removeView(this);
    }

    void setDocument(Document& doc)
    {
        document = &doc;
        document->addView(this);
        update();
    }

    virtual void update() = 0;

protected:
    Document* document;
};

template<typename T>
using ViewFunction = std::function<T&(BowParameters&)>;

template<typename T>
class View: public ViewBase
{
public:
    View(ViewFunction<T> view_function)
        : view_function(view_function)
    {

    }

    T& getData()
    {
        return view_function(document->getData());  // Todo: Check for nullptr
    }

    const T& getConstData()
    {
        return view_function(document->getData());  // Todo: Check for nullptr
    }

private:
    ViewFunction<T> view_function;
};
