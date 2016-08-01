#include "Document.hpp"

BowParameters& Document::getData()
{
    return data;
}

void Document::addView(ViewBase* view)
{
    views.insert(view);
    view->update();
}

void Document::removeView(ViewBase* view)
{
    views.erase(view);
}

void Document::updateViews()
{
    for(auto view: views)
    {
        view->update();
    }
}

