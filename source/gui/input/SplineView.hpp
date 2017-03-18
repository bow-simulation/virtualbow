#pragma once
#include "gui/Plot.hpp"
#include "model/Document.hpp"

class InputData;

class SplineView: public Plot
{
public:
    SplineView(const QString& lbx, const QString& lby, DocItem<Series>& doc_item);

private:
    Connection connection;
};
