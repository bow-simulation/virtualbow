#pragma once
#include "../Plot.hpp"
#include "../../model/Document.hpp"

class InputData;

class SplineView: public Plot
{
public:
    SplineView(const QString& lbx, const QString& lby, DocItem<Series>& doc_item);

private:
    Connection connection;
};

class ProfileView: public Plot
{
public:
    ProfileView(InputData& data);

private:
    InputData& data;
    std::vector<Connection> connections;

    void update();
};
