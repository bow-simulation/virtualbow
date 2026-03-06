#pragma once
#include <QWidget>

class QGridLayout;
class QLabel;

class PropertyList: public QWidget {
public:
    PropertyList(QWidget* parent = nullptr);

    int addHeading(const QString& title);
    int addProperty(const QString& name, QWidget* editor);
    int addWidget(QWidget* widget);
    int addStretch();

    void setVisible(int row, bool value);

private:
    QGridLayout* grid;
};
