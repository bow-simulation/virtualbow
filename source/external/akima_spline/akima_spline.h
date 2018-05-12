/*
 * akima_spline.h
 *
 * a simple akima spline interpolation library without external
 * dependencies
 *
 * ---------------------------------------------------------------------
 * Copyright (C) 2018, CodingSmith (wangbfslingman at gmail.com)
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * ---------------------------------------------------------------------
 *
 */


#ifndef AKIMA_SPLINE_H
#define AKIMA_SPLINE_H

#include <cstdio>
#include <cassert>
#include <vector>
#include <algorithm>
#include <iostream>
#include <math.h>
#include <iterator>

// unnamed namespace only because the implementation is in this
// header file and we don't want to export symbols to the obj files

namespace cs
{

class AkimaSplineBase {
public:
    AkimaSplineBase(){};
    AkimaSplineBase(const std::vector<float> &x, const std::vector<float> &y){
                    set_points(x, y);
                };

    void set_points(const std::vector<float> &x, const std::vector<float> &y);
    bool generate_points(std::vector<float> &x, std::vector<float> &y);
    bool constructed_flag;
    
private:
    std::vector<float> raw_slopes, argmented_slopes;
    std::vector<float> m_x, m_y;
    std::vector<float> m_b, m_c, m_d;
    std::vector<int> num_count, bin_idx;
    std::vector<float> diff_x, diff_y;
    std::vector<float> diff_argmented_slopes;
    std::vector<float> f1, f2, f12;
    int data_length;

    void diff(std::vector<float> &val, std::vector<float> &diff_val);
    bool validity_check();
    void hist_count(std::vector<float> &x, std::vector<int> &num_count, std::vector<int> &bin, std::vector<int> &bb);
};


class AkimaSpline {
public:
    AkimaSpline(){};
    AkimaSpline(std::vector<float> &x, \
                std::vector<float> &y, \
                int &t_num){
        _m_x = x;
        _m_y = y;
        _t_num = t_num;
        generate_t_internal();
        generate_t_out();
    };

    ~AkimaSpline(){};

    void generate_t_internal();
    void generate_t_out();
    bool init(std::vector<float> &x, std::vector<float> &y, int t_num);
    void proc();
    bool validity_check();
    bool constructed_flag;
        
    std::vector<float> x_out;
    std::vector<float> y_out;
    
private:
    AkimaSplineBase _x_spline;
    AkimaSplineBase _y_spline;
    std::vector<float> _t_internal;
    std::vector<float> _t_out;
    std::vector<float> _x_out;
    std::vector<float> _y_out;
    std::vector<float> _m_x, _m_y;
    int _data_length;
    int _t_num;
};

} // namespace cs


#endif /* TK_SPLINE_H */
