/*
#include <QtWidgets>

QPixmap drawLayerSymbol(const QColor& color)
{
    // https://stackoverflow.com/a/29196812/4692009
    // https://stackoverflow.com/a/37213313/4692009

    const double size = 48.0;

    QPixmap pixmap(size, size);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    //painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.fillRect(QRectF{0.0, 0.0, size, size}, color);

    //QPainterPath path;
    //path.addRoundedRect(QRectF(0, 0, size, size), 0.3*size, 0.3*size);

    //painter.setPen(Qt::NoPen);
    //painter.fillPath(path, color);
    //painter.drawPath(path);

    return pixmap;
}

class LegendEntry: public QWidget
{
public:
    LegendEntry()
        : symbol(new QLabel()),
          label(new QLabel())
    {
        int size = symbol->sizeHint().height();
        symbol->setFixedWidth(size);
        symbol->setFixedHeight(size);
        symbol->setScaledContents(true);

        auto hbox = new QHBoxLayout();
        hbox->addWidget(symbol, 0);
        hbox->addWidget(label, 1);
        this->setLayout(hbox);
    }

    void setColor(const QColor& color)
    {
        symbol->setPixmap(drawLayerSymbol(color));
    }

    void setText(const QString& text)
    {
        label->setText(text);
    }


private:
    QLabel* symbol;
    QLabel* label;
};


int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    LegendEntry entry;
    entry.setColor(QColor{150, 170, 80});
    entry.setText("Layer 123");
    entry.show();

    return app.exec();
}
*/

#include "gui/Application.hpp"

int main(int argc, char* argv[])
{
    return Application::run(argc, argv);
}

/*
#include <QtWidgets>
#include <random>

QColor getColor(double rho, double E)
{
    std::mt19937 rng(rho*E);    // random-number engine used (Mersenne-Twister in this case)
    std::uniform_int_distribution<int> uni(0, 255); // guaranteed unbiased

    return QColor(uni(rng), uni(rng), uni(rng));
}

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QLabel label("ThisIsATestWithVeryLongText");
    label.show();

    double E = 1.2e12;
    double rho = 7500;

    QPalette palette = label.palette();
    palette.setColor(label.backgroundRole(), getColor(rho, E));
    label.setPalette(palette);

    return app.exec();
}
*/

/*
#include "gui/EditableTabBar.hpp"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    // QApplication::setStyle("windows");

    EditableTabBar tb;
    tb.addTab("Layer0");
    tb.addTab("Layer1");
    tb.addTab("Layer");
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
