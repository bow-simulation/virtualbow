/*
template<typename T>
class View
{
public:
    View(T& data): data(data), cache(data)
    {

    }

    void update()
    {
        if(data != cache)
        {
            updateView(data);
            cache = data;
        }
    }

protected:
    void updateData(const T& data)
    {
        this->data = data;
        cache = data;
    }

    virtual void updateView(const T& data) = 0;

private:
    T& data;
    T cache;
};


class DataView: public View<Data>
{
public:
    DataView(Data& data): View(data)
    {

    }

    virtual void updateView(const Data& data) override
    {

    }
};
*/

#include "gui/Application.hpp"

int main(int argc, char* argv[])
{
    return Application::run(argc, argv);
}

/*
#include "gui/EditableTabBar.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    // QApplication::setStyle("windows");

    EditableTabBar tb;
    tb.addTab("Layer0");
    tb.addTab("Layer1");
    tb.addTab("Layer2");
    tb.show();

    QObject::connect(&tb, &EditableTabBar::addTabRequested, []()
    {
        qInfo() << "Add Tab!";
    });

    QObject::connect(&tb, &EditableTabBar::tabRenamed, [](int index, const QString& name)
    {
        qInfo() << index << " renamed to " << name;
    });

    return app.exec();
}
*/
