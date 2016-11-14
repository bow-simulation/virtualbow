#pragma once
#include <QtWidgets>

class InputData;

class BowEditor: public QWidget
{
public:
    BowEditor(InputData& data);
};

class ProfileEditor: public QWidget
{
public:
    ProfileEditor(InputData& data);
};

class WidthEditor: public QWidget
{
public:
    WidthEditor(InputData& data);
};

class HeightEditor: public QWidget
{
public:
    HeightEditor(InputData& data);
};
