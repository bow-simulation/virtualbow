#include "Document.hpp"

Document::Document(): modified(false)
{

}

void Document::load(const std::string& path)
{
    data.load(path);
    modified = false;
    updateViews();
}

void Document::save(const std::string& path)
{
    data.save(path);
    modified = false;
}

const BowParameters& Document::getData()
{
    return data;
}

void Document::addView(ViewBase* view)
{
    views.insert(view);
}

void Document::removeView(ViewBase* view)
{
    views.erase(view);
}

void Document::updateViews()
{
    for(auto view: views)
    {
        view->updateView();
    }
}

