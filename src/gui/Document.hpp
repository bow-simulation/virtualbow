#pragma once
#include <vector>
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
        views.push_back(view);
        view->update();
    }

    void removeView(ViewBase* view)
    {
        auto it = std::find(views.begin(), views.end(), view);
        if(it != views.end())
            views.erase(it);
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
    std::vector<ViewBase*> views;

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

    /*
    virtual void update() override
    {
        update(view_function(document.getData()));
    }

    virtual void update(const T& data) = 0;
    */

protected:
    Document document;

private:
    ViewFunction<T> view_function;
};
