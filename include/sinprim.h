#ifndef PRIM_H
#define PRIM_H

class sphere;

////////////////////////////////////////////////////////////////////////////////
//
struct vec3f
{
    float x, y, z;

    vector();
    vector(float,float,float);
    vector operator *(vector);
    vector operator *(float);
    void rotate(float);

    void operator *=(float);
    point operator +(const vector &in) { return point(in.x+x, in.y+y, in.z+z); }
    vector operator -(const point &in) { return vector(x-in.x, y-in.y, z-in.z); }

    //vector functions
    void normalize();
    void createFromPoints( point, point );
    void crossproduct(vector);
    void subtract(vector);
    void set(float,float,float);
    int sphereIntersect(sphere&);
    float dotproduct(vector);
    void mult_by_matrix(float*);
};

////////////////////////////////////////////////////////////////////////////////
//
struct point {

    float x, y, z;

    point(const float &inx, const float &iny, const float &inz) { x=inx; y=iny; z=inz; }
    point() { x=0; y=0; z=0; }

    change all this to vector

    point operator -(const point &in);

    point operator +(const point &in);
    point operator /(const point &in);

    void set(const float &inx, const float &iny, const float &inz) { x=inx; y=iny; z=inz; }
    float distance( const point &in) { return ((x-in.x)*(x-in.x) + (y-in.y)*(y-in.y) + (z-in.z)*(z-in.z)); }
    float distance( const float &inx, const float &iny, const float &inz);
    void ray_point_distance( vector , point , point * );
    float ray_point_distance( vector , point );
    void mult_by_matrix(float*);

};

////////////////////////////////////////////////////////////////////////////////
//
struct sphere {

    point pos;
    float radius;

    sphere(float,float,float,float);

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
    void createFromAxisAngle( float, float, float, float );
    void createMatrix( float* );
    vector toVector();
};


#endif //\/ PRIM_H
