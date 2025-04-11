#pragma once
#include <QWidget>

class QGridLayout;

class PropertyList: public QWidget {
public:
    PropertyList(QWidget* parent = nullptr);

    void addHeading(const QString& title);
    void addProperty(const QString& name, const QString& tooltip, QWidget* widget);
    void addWidget(const QString& tooltip, QWidget* widget);
    void addStretch();

private:
    QGridLayout* grid;
};
