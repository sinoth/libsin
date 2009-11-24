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
    void mult_by_matrix(float *in) { static float x2,y2,z2;
                                     x2 = x*in[0] + y*in[4] + z*in[8];
                                     y2 = x*in[1] + y*in[5] + z*in[9];
                                     z2 = x*in[2] + y*in[6] + z*in[10];
                                     x=x2; y=y2; z=z2; }

    void ray_point_distance( vec3f , vec3f , vec3f * );
    float ray_point_distance( vec3f , vec3f );

    int sphereIntersect(sphere &insphere) { normalize();
                                            //if ( dotproduct(insphere.pos)*dotproduct(insphere.pos) -
                                            return 0;
                                          }

};


////////////////////////////////////////////////////////////////////////////////
// sphere shizzle
////////////////////////////////////////////////////////////////////////////////
//
struct sphere {

    vec3f pos;
    float radius;

    sphere(const vec3f &inv, const float &inr) { pos = inv; radius = inr; }
    sphere(const float &inx, const float &iny, const float &inz, const float &inr)
                                               { pos.set(inx,iny,inz); radius=inr; }

};


//////////////////////////////////////////
// Quaternion camera code from Vic Hollis
// http://nehe.gamedev.net/data/lessons/lesson.asp?lesson=Quaternion_Camera_Class
//////////////////////////////////////////////////////////////////////////////////
//
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

    quaternion operator *(const quaternion &in) { return quaternion( d_w*in.d_x + d_x*in.d_w + d_y*in.d_z - d_z*in.d_y,
	                                                                 d_w*in.d_y + d_y*in.d_w + d_z*in.d_x - d_x*in.d_z,
	                                                                 d_w*in.d_z + d_z*in.d_w + d_x*in.d_y - d_y*in.d_x,
	                                                                 d_w*in.d_w - d_x*in.d_x - d_y*in.d_y - d_z*in.d_z ); }

    void operator *=(const quaternion &in) { *this = quaternion( d_w*in.d_x + d_x*in.d_w + d_y*in.d_z - d_z*in.d_y,
	                                                             d_w*in.d_y + d_y*in.d_w + d_z*in.d_x - d_x*in.d_z,
	                                                             d_w*in.d_z + d_z*in.d_w + d_x*in.d_y - d_y*in.d_x,
	                                                             d_w*in.d_w - d_x*in.d_x - d_y*in.d_y - d_z*in.d_z ); }

    double length() { return sqrt(d_x*d_x + d_y*d_y + d_z*d_z + d_w*d_w); }
    double length_sq() { return (d_x*d_x + d_y*d_y + d_z*d_z + d_w*d_w); }

    void normalize() { static double R; R = sqrt(d_w*d_w + d_x*d_x + d_y*d_y + d_z*d_z);
                       d_w /= R; d_x /= R; d_y /= R; d_z /= R; }

    void conjugate() { d_x = -d_x; d_y = -d_y; d_z = -d_z; }

    vec3f toVec3f() { return vec3f( (d_x * (d_z + d_z))+(d_w * (d_y + d_y)),
                                    (d_y * (d_z + d_z))-(d_w * (d_x + d_x)),
                                    1 - ((d_x * (d_x + d_x))+(d_y * (d_y + d_y))) ); }

    void toMatrix( float *mat ) {
                            // creates 4x4 homogeneous matrix that can be applied to an OpenGL Matrix
                            // Make sure the matrix has allocated memory to store the rotation data
                            if(!mat) return;
                            // First row
                            mat[ 0] = 1.0f - (2.0f * ( d_y * d_y + d_z * d_z ));
                            mat[ 1] = 2.0f * (d_x * d_y - d_z * d_w);
                            mat[ 2] = 2.0f * (d_x * d_z + d_y * d_w);
                            //mat[ 3] = 0.0f;
                            // Second row
                            mat[ 4] = 2.0f * ( d_x * d_y + d_z * d_w );
                            mat[ 5] = 1.0f - (2.0f * ( d_x * d_x + d_z * d_z ));
                            mat[ 6] = 2.0f * (d_z * d_y - d_x * d_w );
                            //mat[ 7] = 0.0f;
                            // Third row
                            mat[ 8] = 2.0f * ( d_x * d_z - d_y * d_w );
                            mat[ 9] = 2.0f * ( d_y * d_z + d_x * d_w );
                            mat[10] = 1.0f - (2.0f * ( d_x * d_x + d_y * d_y ));
                            //mat[11] = 0.0f;
                            // Fourth row
                            //mat[12] = 0; mat[13] = 0; mat[14] = 0; mat[15] = 1.0f;
                            }

    void createFromAxisAngle(const float &xin, const float &yin, const float &zin, const float &degrees) {
                                static float result;
                                result = (float)sin( degrees * 0.00872664625f );
                                // Calcualte the w value by cos( theta / 2 )
                                d_w = (float)cos( degrees * 0.00872664625f );
                                // Calculate the x, y and z of the quaternion
                                d_x = xin * result;
                                d_y = yin * result;
                                d_z = zin * result; }

};


#endif //\/ PRIM_H



/*
    float* toMatrix() { float* theMatrix = new float[16];
                        theMatrix[0] = 1 - (2*d_y)*(2*d_y) - (2*d_z)*(2*d_z);
                        theMatrix[4] = 2*d_x*d_y - 2*d_w*d_z;
                        theMatrix[8] = 2*d_x*d_z + 2*d_w*d_y;
                        theMatrix[12]= 0;

                        theMatrix[1] = 2*d_x*d_y + 2*d_w*d_z;
                        theMatrix[5] = 1 - (2*d_x)*(2*d_x) - (2*d_z)*(2*d_z);
                        theMatrix[9] = 2*d_y*d_z - 2*d_w*d_x;
                        theMatrix[13]= 0;

                        theMatrix[2] = 2*d_x*d_z - 2*d_w*d_y;
                        theMatrix[6] = 2*d_y*d_z + 2*d_w*d_x;
                        theMatrix[10]= 1 - (2*d_x)*(2*d_x) - (2*d_y)*(2*d_y);
                        theMatrix[14]= 0;

                        theMatrix[3] = 0;
                        theMatrix[7] = 0;
                        theMatrix[11]= 0;
                        theMatrix[15]= 1;
                        return theMatrix; }
*/
/*
vector quaternion::toVector() {
    float x2 = d_x + d_x, y2 = d_y + d_y, z2 = d_z + d_z;
    float xx = d_x * x2, xz = d_x * z2;
    float yy = d_y * y2, yz = d_y * z2;
    float wx = d_w * x2, wy = d_w * y2;
    return vector( xz+wy, yz-wx, 1 - (xx+yy) );
}
*/
/*
void point::ray_point_distance( vector ray_dir, point check, point *best_point ) {

	static vector t;
	static float dot;

	t.x = check.x - x;
	t.y = check.y - y;
	t.z = check.z - z;

	dot = t.dotproduct(ray_dir);

	if ( dot < 0.0 ) dot = 0.0;

	*best_point = *this + (ray_dir * dot);

    //printf("ray_start: %f,%f,%f\n",ray_start.x,ray_start.y,ray_start.z);
    //printf("ray_dir: %f,%f,%f\n---\n",ray_dir.x,ray_dir.y,ray_dir.z);
    //printf("line: %f,%f,%f\n",best_point.x,best_point.y,best_point.z);
    //printf("check: %f,%f,%f\n\n",check.x,check.y,check.z);
	//return fabs(best_point.distance(check));
}
*/
/*

void quaternion::createFromAxisAngle(float xin, float yin, float zin, float degrees)
{
	//static const GLdouble pi = 3.14159265;

	// First we want to convert the degrees to radians
	// since the angle is assumed to be in radians
	static float angle;
	static float result;

	//angle = float((degrees / 180.0f) * 3.14159265);
	angle = degrees * 0.0174532925;

	// Here we calculate the sin( theta / 2) once for optimization
	result = (float)sin( angle / 2.0f );

	// Calcualte the w value by cos( theta / 2 )
	d_w = (float)cos( angle / 2.0f );

	// Calculate the x, y and z of the quaternion
	d_x = float(xin * result);
	d_y = float(yin * result);
	d_z = float(zin * result);
}
*/
