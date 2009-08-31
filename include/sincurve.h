#ifndef CURVE_H
#define CURVE_H

#include <sinprim.h>
#include <vector>


class nurbs_curve {
public:


    std::vector<point> control_points;
    std::vector<float> knots;

    int degree;
    int order;
    float max_knot;
    int size;

    nurbs_curve();
    void add_control_point( point );
    void add_control_point( float,float,float );
    void add_control_point( float,float );
    void calculate_path(unsigned int);
    float CoxDeBoor(float ,int ,int );
    void GetOutpoint(float t, point*);
    void info();

};



#endif

