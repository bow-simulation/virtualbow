#pragma once
#include <QtWidgets>

class InputData;

class BowEditor: public QSplitter
{
public:
    BowEditor(InputData& data);
};

class GeneralEditor: public QWidget
{
public:
    GeneralEditor(InputData& data);
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
