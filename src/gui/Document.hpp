#pragma once
#include <set>
#include <functional>
#include <algorithm>

#include "../model/BowParameters.hpp"

#include <QtCore>

class ViewBase
{
public:
    virtual void update() = 0;
};

class Document
{
public:
    BowParameters& getData()
    {
        return data;
    }

    void addView(ViewBase* view)
    {
        views.insert(view);
        view->update();
    }

    void removeView(ViewBase* view)
    {
        views.erase(view);
    }

    void updateViews()
    {
        for(auto view: views)
        {
            view->update();
        }
    }

private:
    BowParameters data;
    std::set<ViewBase*> views;
};

template<typename T>
using ViewFunction = std::function<T&(BowParameters&)>;

template<typename T>
class View: public ViewBase
{
public:
    View(Document& document, ViewFunction<T> view_function)
        : document(document),
          view_function(view_function)
    {

    }

    T& getData()
    {
        return view_function(document.getData());
    }

    void setData(const T& data)
    {
        view_function(document.getData()) = data;
    }

protected:
    Document& document;

private:
    ViewFunction<T> view_function;
};
