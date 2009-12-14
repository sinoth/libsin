#ifndef CURVE_H
#define CURVE_H

#include <sinprim.h>
#include <vector>

///////////////////////////////////////////////////////
// algorithms based on the following site:
// http://www.robthebloke.org/opengl_programming.html
///////////////////////////////////////////////////////
class nurbs_curve
{
private:

    int degree;
    int order;
    int size;
    float max_knot;

    std::vector<vec3f> control_points;
    std::vector<float> knots;


public:

    nurbs_curve() : degree(3), order(degree+1) {}


////////////////////////////////////////////////////
    void add_control_point( const vec3f &inpoint ) {
            control_points.push_back(inpoint);
            size = control_points.size();

            if ( size >= 4 ) {
                knots.clear();
                int i;
                float j;

                for ( i=0, j=0; i < size+order; i++ ) {
                    if ( i > 3 && i <= size )
                        j++;
                    knots.push_back(j);
                }
                max_knot = j;
            }
        }


//////////////////////////////////////////////////////////////////
    float CoxDeBoor(const float &u, const int &i, const int &k ) {
            if(k==1) {
                if( knots[i] <= u && u <= knots[i+1] )
                    { return 1.0f; }
                return 0.0f;
            }

            float Den1, Den2, Eq1, Eq2;

            Eq1=0, Eq2=0;
            Den1 = knots[i+k-1] - knots[i];
            Den2 = knots[i+k] - knots[i+1];

            if(Den1>0) { Eq1 = ((u-knots[i]) / Den1) * CoxDeBoor(u,i,k-1); }
            if(Den2>0) { Eq2 = (knots[i+k]-u) / Den2 * CoxDeBoor(u,i+1,k-1); }

            return Eq1+Eq2;
        }


////////////////////////////////////////////////////////
    void GetOutpoint(const float &t, point *OutPoint ) {
            OutPoint->set(0.0,0.0,0.0);

            // sum the effect of all CV's on the curve at this point to
            // get the evaluated curve point
            //
            for(int i=0;i!=size;++i) {

                // calculate the effect of this point on the curve
                float Val = CoxDeBoor(t,i,order);

                if(Val>0.001f) {
                    // sum effect of CV on this part of the curve
                    OutPoint->x += Val * control_points[i].x;
                    OutPoint->y += Val * control_points[i].y;
                    OutPoint->z += Val * control_points[i].z;
                }
            }
        }

    //void calculate_path(unsigned int); ???

};



#endif





/*
float g_Points[7][3] = {
	{ 10,10,0 },
	{  5,10,2 },
	{ -5,5,0 },
	{-10,5,-2},
	{-4,10,0},
	{-4,5,2},
	{-8,1,0}
};
*/

/*
float g_Knots[] = {0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 4.0f, 4.0f, 4.0f};
//float g_Knots[] = {0.0f,0.0f,0.0f,1.0f,2.0f,3.0f,4.0f,5.0f,6.0f,6.0f,6.0f};
unsigned int g_num_cvs=7;
unsigned int g_degree=3;
unsigned int g_order=g_degree+1;
unsigned int g_num_knots=g_num_cvs+g_order;

unsigned int LOD=20;
*/

