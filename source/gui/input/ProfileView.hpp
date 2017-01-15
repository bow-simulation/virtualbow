#pragma once
#include "../Plot.hpp"
#include "../../model/Document.hpp"

class InputData;

class ProfileView: public Plot
{
public:
    ProfileView(InputData& data);

private:
    InputData& data;
    std::vector<Connection> connections;

    void update();
};
