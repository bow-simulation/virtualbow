#pragma once
#include <set>
#include <functional>
#include <algorithm>

#include "../model/BowParameters.hpp"

#include <QtCore>


template<typename T>
using ViewFunction = std::function<T&(BowParameters&)>;
using UpdateFunction = std::function<void()>;

class ViewBase
{
public:
    UpdateFunction update;
    ViewBase(UpdateFunction update): update(update)
    {

    }
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
class View: public ViewBase
{
public:
    View(Document& document, ViewFunction<T> view_function, UpdateFunction update_function)
        : ViewBase(update_function),
          document(document),
          view_function(view_function)
    {
        document.addView(this);
    }

    ~View()
    {
        document.removeView(this);
    }

    T& getData()
    {
        return view_function(document.getData());
    }

    void setData(const T& data)
    {
        view_function(document.getData()) = data;
    }

private:
    Document& document;
    ViewFunction<T> view_function;
};
