#pragma once
#include <QWidget>

class QGridLayout;

class PropertyList: public QWidget {
public:
    PropertyList(QWidget* parent = nullptr);

    void addHeading(const QString& title);
    void addProperty(const QString& name, QWidget* editor);
    void addWidget(QWidget* widget);
    void addStretch();

private:
    QGridLayout* grid;
};
