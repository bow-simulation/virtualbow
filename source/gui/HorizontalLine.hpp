#pragma once
#include <QtWidgets>

class HorizontalLine: public QFrame
{
public:
    HorizontalLine()
    {
        this->setFrameShape(QFrame::HLine);
        this->setFrameShadow(QFrame::Sunken);
        this->setContentsMargins(15, 0, 15, 0);    // Magic numbers
    }
};
