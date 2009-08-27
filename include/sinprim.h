#ifndef PRIM_H
#define PRIM_H

#include <GL/glfw.h>

class point;

////////////////////////////////////////////////////////////////////////////////
//
class colour {
public:

    float r;
    float g;
    float b;
    float a;

    colour(float,float,float,float);
    colour();

    void draw();
    void set(float,float,float,float);

};


////////////////////////////////////////////////////////////////////////////////
//
struct vector
{
    double x, y, z;
    vector();
    vector(double,double,double);
    vector operator *(vector);
    vector operator *(float);
    void rotate(float);

    void operator *=(float);

    //vector functions
    void normalize();
    void createFromPoints( point, point );
    void crossproduct(vector);
    void subtract(vector);
    void set(double,double,double);
    float dotproduct(vector);
    void mult_by_matrix(GLfloat*);
    void print(char *);
};


////////////////////////////////////////////////////////////////////////////////
//
class point {
public:

    float x;
    float y;
    float z;
    //colour my_color;

    point(float,float,float);
    point(int,int,int);
    point();
    point operator +(vector);
    vector subToVector(point);
    point operator -(point);
    point operator +(point);
    point operator /(point);

    void draw();
    void set(float,float,float);
    float distance( point );
    float distance( float,float,float );
    void ray_point_distance( vector , point , point * );
    float ray_point_distance( vector , point );
    void mult_by_matrix(GLfloat*);



};



////////////////////////////////////////////////////////////////////////////////
//
struct quaternion
{
    double d_x, d_y, d_z, d_w;
    quaternion();
    quaternion(double,double,double,double);
    quaternion operator *(quaternion);

    //quaternion functions
    double length();
    void normalize();
    void conjugate();
    void mult(quaternion);
    float* toMatrix();
    void createFromAxisAngle( GLfloat, GLfloat, GLfloat, GLfloat );
    void createMatrix( GLfloat* );
    vector toVector();
};


#endif //\/ PRIM_H
