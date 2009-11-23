#ifndef PRIM_H
#define PRIM_H

#include <math.h>

struct sphere;

////////////////////////////////////////////////////////////////////////////////
// vector shizzle
////////////////////////////////////////////////////////////////////////////////
//
struct vec3f
{
    float x, y, z;

    vec3f() { x=0.0; y=0.0; z=0.0; }
    vec3f(const float &inx, const float &iny, const float &inz) { x=inx; y=iny; z=inz; }
    void set(const float &inx, const float &iny, const float &inz) { x=inx; y=iny; z=inz; }

    void  operator +=(const vec3f &in) { x += in.x; y += in.y; z += in.z; }
    void  operator -=(const vec3f &in) { x -= in.x; y -= in.y; z -= in.z; }
    void  operator *=(const float &in) { x *= in; y *= in; z *= in; }
    void  operator /=(const float &in) { x /= in; y /= in; z /= in; }

    vec3f operator +(const vec3f &in) { return vec3f(x+in.x, y+in.y, z+in.z); }
    vec3f operator -(const vec3f &in) { return vec3f(x-in.x, y-in.y, z-in.z); }
    vec3f operator *(const float &in) { return vec3f(x*in, y*in, z*in); }
    vec3f operator /(const float &in) { return vec3f(x/in, y/in, z/in); }

    float dot(const vec3f &in) { return x*in.x + y*in.y + z*in.z; }

    vec3f cross(const vec3f &in) { return vec3f( y * in.z - z * in.y,
                                                 z * in.x - x * in.z,
                                                 x * in.y - y * in.x ); }

    float distance_sq( const vec3f &in) { return ((x-in.x)*(x-in.x) + (y-in.y)*(y-in.y) + (z-in.z)*(z-in.z)); }
    float distance( const vec3f &in) { return sqrtf((x-in.x)*(x-in.x) + (y-in.y)*(y-in.y) + (z-in.z)*(z-in.z)); }

    void rotate2D(const float &indegrees) { float rad = (indegrees*0.0174532925);
                                            float tx = (x * cosf(rad)) - (y * sinf(rad));
                                            float ty = (x * sinf(rad)) + (y * cosf(rad));
                                            x = tx; y = ty; }

    void normalize() { static float ln; ln = sqrt( x*x + y*y + z*z );
                       if (ln == 0) return; x /= ln; y /= ln; z /= ln; }

    void createFromPoints( const vec3f &a, const vec3f &b) { x = b.x - a.x;
                                                             y = b.y - a.y;
                                                             z = b.z - a.z; }
    void mult_by_matrix(float*) { static float x2,y2,z2;
                                  x2 = x*in[0] + y*in[4] + z*in[8];
                                  y2 = x*in[1] + y*in[5] + z*in[9];
                                  z2 = x*in[2] + y*in[6] + z*in[10];
                                  x=x2; y=y2; z=z2; }

    void ray_point_distance( vector , point , point * );
    float ray_point_distance( vector , point );

    int sphereIntersect(sphere &insphere) { normalize();
                                            if ( dotproduct(insphere.pos)*dotproduct(insphere.pos) -
                                          }

};


////////////////////////////////////////////////////////////////////////////////
// sphere shizzle
////////////////////////////////////////////////////////////////////////////////
//
struct sphere {

    vec3f pos;
    float radius;

    sphere(const vec3f &inv, const float *inr) { pos = inv; radius = inr; }
    sphere(const float &inx, const float &iny, const float &inz, const float &inr)
                                               { pos.set(inx,iny,inz); radius=inr; }

};


////////////////////////////////////////////////////////////////////////////////
// quaternion hipster shizzle
////////////////////////////////////////////////////////////////////////////////
//
struct quaternion
{
    double d_x, d_y, d_z, d_w;

    quaternion() { d_x=0.0; d_y=0.0; d_z=0.0; d_w=1.0; }
    quaternion(const double &xin, const double &yin, const double &zin, const double &win )
                                            { d_x = xin; d_y = yin; d_z = zin; d_w = win; }

    quaternion operator *(quaternion);

    //quaternion functions
    double length() { return sqrt(d_x*d_x + d_y*d_y + d_z*d_z + d_w*d_w); }
    double length_sq() { return (d_x*d_x + d_y*d_y + d_z*d_z + d_w*d_w); }
    void normalize();
    void conjugate() { d_x = -d_x; d_y = -d_y; d_z = -d_z; }
    void mult(quaternion);
    float* toMatrix();
    void createFromAxisAngle( float, float, float, float );
    void createMatrix( float* );
    vec3f toVec3f();
};


#endif //\/ PRIM_H
