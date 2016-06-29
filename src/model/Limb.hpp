#pragma once
#include <vector>

struct LimbDef
{

};


struct Limb
{
    std::vector<double> s;
    std::vector<double> x;
    std::vector<double> y;
    std::vector<double> phi;
    std::vector<double> h;
    std::vector<double> Cee;
    std::vector<double> Ckk;
    std::vector<double> Cek;
    std::vector<double> rhoA;

    Limb()
    {

    }
};
