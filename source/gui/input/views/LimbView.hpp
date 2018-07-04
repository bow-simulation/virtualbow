#pragma once
#include "bow/input/InputData.hpp"
#include <QtWidgets>

class LimbView: public QLabel
{
public:
    LimbView();
    void setData(const InputData& data);

    void viewProfile();
    void viewTop();
    void view3D();
    void viewSymmetric(bool checked);
    void viewFit();
};
