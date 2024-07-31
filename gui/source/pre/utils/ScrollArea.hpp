#pragma once
#include <QScrollArea>

// Helper function for wrapping a widget in a scroll area
inline QScrollArea* scrollArea(QWidget* widget) {
    auto scroll = new QScrollArea();
    scroll->setWidget(widget);
    return scroll;
}
