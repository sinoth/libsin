#ifndef SINPRIM_H
#define SINPRIM_H

#include <math.h>
#include <list>
#include <vector>

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
    void  operator *=(const float *in) { float x2,y2,z2;
                                         x2 = x*in[0] + y*in[4] + z*in[8];
                                         y2 = x*in[1] + y*in[5] + z*in[9];
                                         z2 = x*in[2] + y*in[6] + z*in[10];
                                         x=x2; y=y2; z=z2; }
    void  operator /=(const float &in) { x /= in; y /= in; z /= in; }

    bool  operator ==(const vec3f &in) const { return ( x==in.x && y==in.y && z==in.z); }

    vec3f operator +(const vec3f &in) const { return vec3f(x+in.x, y+in.y, z+in.z); }
    vec3f operator -(const vec3f &in) const { return vec3f(x-in.x, y-in.y, z-in.z); }
    vec3f operator -() const { return vec3f(-x, -y, -z); }
    vec3f operator *(const float &in) const { return vec3f(x*in, y*in, z*in); }
    vec3f operator /(const float &in) const { return vec3f(x/in, y/in, z/in); }

    float dot(const vec3f &in) const { return x*in.x + y*in.y + z*in.z; }
    float operator *(const vec3f &in) const { return x*in.x + y*in.y + z*in.z; }

    void setBySubtract(const vec3f &in1, const vec3f &in2) { x=in1.x-in2.x; y=in1.y-in2.y; z=in1.z-in2.z; }

    float sq() const { return dot(*this); }

    float length() const { return sqrt(x*x+y*y+z*z); }
    float length_sq() const { return x*x+y*y+z*z; }

    vec3f cross(const vec3f &in) const { return vec3f( y * in.z - z * in.y,
                                                       z * in.x - x * in.z,
                                                       x * in.y - y * in.x ); }

    float distance_sq( const vec3f &in) const { return ((x-in.x)*(x-in.x) + (y-in.y)*(y-in.y) + (z-in.z)*(z-in.z)); }
    float distance( const vec3f &in) const { return sqrtf((x-in.x)*(x-in.x) + (y-in.y)*(y-in.y) + (z-in.z)*(z-in.z)); }

    void rotate2D(const float &indegrees) { float rad = (indegrees*0.0174532925);
                                            float tx = (x * cosf(rad)) - (y * sinf(rad));
                                            float ty = (x * sinf(rad)) + (y * cosf(rad));
                                            x = tx; y = ty; }

    void rotate2Dr(const float &inrad) { float tx = (x * cosf(inrad)) - (y * sinf(inrad));
                                         float ty = (x * sinf(inrad)) + (y * cosf(inrad));
                                         x = tx; y = ty; }

    void normalize() { float ln; ln = sqrt( x*x + y*y + z*z );
                       if (ln == 0) return; x /= ln; y /= ln; z /= ln; }

    vec3f norm() { float ln; ln = sqrt( x*x + y*y + z*z );
                   if (ln == 0) return vec3f(x,y,z); return vec3f(x/ln, y/ln, z/ln); }

    void createFromPoints( const vec3f &a, const vec3f &b) { x = b.x - a.x;
                                                             y = b.y - a.y;
                                                             z = b.z - a.z; }
    void mult_by_matrix(float *in) { float x2,y2,z2;
                                     x2 = x*in[0] + y*in[4] + z*in[8];
                                     y2 = x*in[1] + y*in[5] + z*in[9];
                                     z2 = x*in[2] + y*in[6] + z*in[10];
                                     x=x2; y=y2; z=z2; }

    void inv_mult_by_matrix(float *in) { float x2,y2,z2;
                                         x2 = x*in[0] + y*in[1] + z*in[2];
                                         y2 = x*in[4] + y*in[5] + z*in[6];
                                         z2 = x*in[8] + y*in[9] + z*in[10];
                                         x=x2; y=y2; z=z2; }

    void ray_point_distance( vec3f , vec3f , vec3f * );
    float ray_point_distance( vec3f , vec3f );


    vec3f midpoint( const vec3f &in) const { return vec3f( (x + in.x)/2.0, (y + in.y)/2.0, (z + in.z)/2.0 ); }

/*
bool SpherePrimitive::intersect(const Ray& ray, float* t)
{

}
*/

};

struct vec4f
{
    float x, y, z, w;
    vec4f() { x=0.0; y=0.0; z=0.0; w=1.0; }
    vec4f(const float &inx, const float &iny, const float &inz, const float &inw=1.0) { x=inx; y=iny; z=inz; w=inw; }
};

////////////////////////////////////////////////////////////////////////////////
// castin them rays
////////////////////////////////////////////////////////////////////////////////
//
//for colliding :P
struct collision3f {
    vec3f pos;
    float dist;
    bool operator < (const collision3f &in) const { return (dist < in.dist); }
    static bool greaterThan(const collision3f &ina, const collision3f &inb) { return (ina.dist > inb.dist); }
    collision3f() {}
    collision3f(const vec3f &in, const float &ind ) { pos = in; dist = ind; }

};
//////////////
struct ray3f {
   struct vec3f pos;
   struct vec3f dir;

   ray3f( const struct vec3f &inpos, const struct vec3f &indir ) { pos = inpos; dir = indir; }
   ray3f() {}

   //bool collideWithCube( const struct vec3f &incenter, const float &cube_radius, const float &sphere_radius=0, float &return_dist ) {
   bool collideWithCube( struct collision3f &incollision, const float &cube_radius, const float &sphere_radius=0 ) {
            //if we're given a bounding sphere, try it first
            //we might be able to early-out
            //http://www.devmaster.net/wiki/Ray-sphere_intersection

            if ( sphere_radius > 0 ) {
                float len_sq = (incollision.pos - pos) * (incollision.pos - pos);
                float t = (incollision.pos - pos) * dir;
                //sphere is behind ray
                if ( t < 0 ) return 0;
                float half_coord = (sphere_radius*sphere_radius - len_sq) + t*t;
                //misses sphere
                if ( half_coord < 0 ) return 0;
            }


            //check collision with each of the 6 faces
            //http://www.siggraph.org/education/materials/HyperGraph/raytrace/rayplane_intersection.htm

/*
            //left side
            float d = cube_radius;
            float Vd = vec3f(-1,0,0) * dir;
            float Vo = -(vec3f(-1,0,0) * pos + d);
            if ( Vd == 0 ) {//parallel, no intersect
            } else {
                float t = Vo / Vd;
                if ( t < 0 ) return 0;
            }
*/
            //http://www.siggraph.org/education/materials/HyperGraph/raytrace/rtinter3.htm

            vec3f low_point(incollision.pos.x - cube_radius, incollision.pos.y - cube_radius, incollision.pos.z - cube_radius);
            vec3f high_point(incollision.pos.x + cube_radius, incollision.pos.y + cube_radius, incollision.pos.z + cube_radius);
            float t1, t2, tt, tNear, tFar;

            //printf("checking ray dir %.2f %.2f %.2f\n", dir.x,dir.y,dir.z);
            //printf("checking ray pos %.2f %.2f %.2f , dir %.2f %.2f %.2f\n", pos.x,pos.y,pos.z, dir.x,dir.y,dir.z);
            //printf("low %.2f %.2f %.2f , high %.2f %.2f %.2f\n", low_point.x,low_point.y,low_point.z, high_point.x,high_point.y,high_point.z);

            //check the x slab
            tNear = -1000000; tFar = 1000000;
            if ( dir.x == 0 ) { //parallel to plane, so check if it is inside slab
                if ( pos.x < low_point.x || pos.x > high_point.x ) return 0;
            } else {
                t1 = (low_point.x - pos.x) / dir.x;
                t2 = (high_point.x - pos.x) / dir.x;
                if ( t1 > t2 ) //swap them
                    { tt=t1; t1=t2; t2=tt; }
                if ( t1 > tNear ) tNear = t1;
                if ( t2 < tFar ) tFar = t2;
                if ( tNear > tFar ) //missed
                    return 0;
                if ( tFar < 0 ) //box behind ray
                    return 0;
            }

            //check the y slab
            if ( dir.y == 0 ) { //parallel to plane, so check if it is inside slab
                if ( pos.y < low_point.y || pos.y > high_point.y ) return 0;
            } else {
                t1 = (low_point.y - pos.y) / dir.y;
                t2 = (high_point.y - pos.y) / dir.y;
                if ( t1 > t2 ) //swap them
                    { tt=t1; t1=t2; t2=tt; }
                if ( t1 > tNear ) tNear = t1;
                if ( t2 < tFar ) tFar = t2;
                if ( tNear > tFar ) //missed
                    return 0;
                if ( tFar < 0 ) //box behind ray
                    return 0;
            }

            //check the z slab
            if ( dir.z == 0 ) { //parallel to plane, so check if it is inside slab
                if ( pos.z < low_point.z || pos.z > high_point.z ) return 0;
            } else {
                t1 = (low_point.z - pos.z) / dir.z;
                t2 = (high_point.z - pos.z) / dir.z;
                if ( t1 > t2 ) //swap them
                    { tt=t1; t1=t2; t2=tt; }
                if ( t1 > tNear ) tNear = t1;
                if ( t2 < tFar ) tFar = t2;
                if ( tNear > tFar ) //missed
                    return 0;
                if ( tFar < 0 ) //box behind ray
                    return 0;
            }

            //if we're here, it's a hit!
            //printf("hit dist: %f\n", tNear);
            incollision.dist = tNear;

            //determine which side it was hit on
            //if ( tNear

            return 1;

        }
};


////////////////////////////////////////////////////////////////////////////////
// some stuff to make algorithms easier
////////////////////////////////////////////////////////////////////////////////
//
struct edge {
    vec3f p1, p2;
    bool operator==(const edge &in) const { return (p1 == in.p1 && p2 == in.p2) ||
                                                   (p1 == in.p2 && p2 == in.p1);   }
};

struct triangle {
    vec3f p1, p2, p3;
    vec3f t1, t2, t3;
    vec3f norm;
    triangle(const vec3f &inp1, const vec3f &inp2, const vec3f &inp3,
             const vec3f &int1, const vec3f &int2, const vec3f &int3,
             const vec3f &innorm) { p1=inp1; p2=inp2; p3=inp3; t1=int1; t2=int2; t3=int3; norm=innorm; }
    bool operator==(const triangle &in) const { return (p1 == in.p1 && p2 == in.p2 && p3 == in.p3 ) ||
                                                       (p1 == in.p2 && p2 == in.p3 && p3 == in.p1 ) ||
                                                       (p1 == in.p3 && p2 == in.p1 && p3 == in.p2 ) ||
                                                       (p1 == in.p1 && p2 == in.p3 && p3 == in.p2 ) ||
                                                       (p1 == in.p2 && p2 == in.p1 && p3 == in.p3 ) ||
                                                       (p1 == in.p3 && p2 == in.p2 && p3 == in.p1 ); }
};

struct box {
    std::vector<GLfloat> vert;
    std::vector<GLfloat> norm;
    std::vector<GLfloat> tex;
    std::list<triangle>  tri;

    ///////////////////////////////////////////////////////////////
    void draw(float x, float y, float z, float s) {
            //top
            tri.push_back( triangle( vec3f(x-s,y+s,z+s), vec3f(x-s,y+s,z-s), vec3f(x+s,y+s,z-s),
                                     vec3f(0,0,-1), vec3f(0,1,-1), vec3f(1,1,-1), vec3f(0,1,0) ) );
            tri.push_back( triangle( vec3f(x+s,y+s,z-s), vec3f(x+s,y+s,z+s), vec3f(x-s,y+s,z+s),
                                     vec3f(1,1,-1), vec3f(1,0,-1), vec3f(0,0,-1), vec3f(0,1,0) ) );
            //bottom
            tri.push_back( triangle( vec3f(x+s,y-s,z-s), vec3f(x-s,y-s,z-s), vec3f(x-s,y-s,z+s),
                                     vec3f(1,0,-1), vec3f(1,1,-1), vec3f(0,1,-1), vec3f(0,-1,0) ) );
            tri.push_back( triangle( vec3f(x-s,y-s,z+s), vec3f(x+s,y-s,z+s), vec3f(x+s,y-s,z-s),
                                     vec3f(0,1,-1), vec3f(0,0,-1), vec3f(1,0,-1), vec3f(0,-1,0) ) );
            //left
            tri.push_back( triangle( vec3f(x-s,y-s,z+s), vec3f(x-s,y-s,z-s), vec3f(x-s,y+s,z-s),
                                     vec3f(1,0,-1), vec3f(0,0,-1), vec3f(0,1,-1), vec3f(-1,0,0) ) );
            tri.push_back( triangle( vec3f(x-s,y+s,z-s), vec3f(x-s,y+s,z+s), vec3f(x-s,y-s,z+s),
                                     vec3f(0,1,-1), vec3f(1,1,-1), vec3f(1,0,-1), vec3f(-1,0,0) ) );
            //right
            tri.push_back( triangle( vec3f(x+s,y+s,z-s), vec3f(x+s,y-s,z-s), vec3f(x+s,y-s,z+s),
                                     vec3f(1,1,-1), vec3f(0,1,-1), vec3f(0,0,-1), vec3f(1,0,0) ) );
            tri.push_back( triangle( vec3f(x+s,y-s,z+s), vec3f(x+s,y+s,z+s), vec3f(x+s,y+s,z-s),
                                     vec3f(0,0,-1), vec3f(1,0,-1), vec3f(1,1,-1), vec3f(1,0,0) ) );
            //front
            tri.push_back( triangle( vec3f(x+s,y-s,z-s), vec3f(x+s,y+s,z-s), vec3f(x-s,y+s,z-s),
                                     vec3f(1,0,-1), vec3f(1,1,-1), vec3f(0,1,-1), vec3f(0,0,-1) ) );
            tri.push_back( triangle( vec3f(x-s,y+s,z-s), vec3f(x-s,y-s,z-s), vec3f(x+s,y-s,z-s),
                                     vec3f(0,1,-1), vec3f(0,0,-1), vec3f(1,0,-1), vec3f(0,0,-1) ) );
            //back
            tri.push_back( triangle( vec3f(x-s,y+s,z+s), vec3f(x+s,y+s,z+s), vec3f(x+s,y-s,z+s),
                                     vec3f(0,0,-1), vec3f(0,1,-1), vec3f(1,1,-1), vec3f(0,0,1) ) );
            tri.push_back( triangle( vec3f(x+s,y-s,z+s), vec3f(x-s,y-s,z+s), vec3f(x-s,y+s,z+s),
                                     vec3f(1,1,-1), vec3f(1,0,-1), vec3f(0,0,-1), vec3f(0,0,1) ) );
        }

    /////////////////////////
    void optimize() {
            std::list<triangle>::iterator itt;
            //now optimize the triangle list
            for ( std::list<triangle>::iterator it=tri.begin(); it != tri.end(); ++it ) {
                itt=it;
                for ( ++itt; itt != tri.end(); ++itt) {
                    if ( (*it) == (*itt) ) { it = tri.erase(it); it--; tri.erase(itt); break; }
                }
            }
            //dump the triangle list into the box lists
            for ( std::list<triangle>::iterator it=tri.begin(); it != tri.end(); ++it ) {
                vert.push_back((*it).p1.x); vert.push_back((*it).p1.y); vert.push_back((*it).p1.z);
                vert.push_back((*it).p2.x); vert.push_back((*it).p2.y); vert.push_back((*it).p2.z);
                vert.push_back((*it).p3.x); vert.push_back((*it).p3.y); vert.push_back((*it).p3.z);
                norm.push_back((*it).norm.x); norm.push_back((*it).norm.y); norm.push_back((*it).norm.z);
                norm.push_back((*it).norm.x); norm.push_back((*it).norm.y); norm.push_back((*it).norm.z);
                norm.push_back((*it).norm.x); norm.push_back((*it).norm.y); norm.push_back((*it).norm.z);
                tex.push_back((*it).t1.x); tex.push_back((*it).t1.y);
                tex.push_back((*it).t2.x); tex.push_back((*it).t2.y);
                tex.push_back((*it).t3.x); tex.push_back((*it).t3.y);
            }
            //clear the triangle list
            tri.clear();
        }
};

struct util {
static int draw_circle ( const vec3f &inpos, const vec4f &incolor, const int &inradius, const int &indetail, std::vector<float> *vec_vert, std::vector<float> *vec_color ) {

    //start with straight up vector
    //start at angle of 90 degrees
    //based on detail factor divide 90 to be smaller
    //return number of points added

    vec3f prev_point(0,1,0), next_point(0,1,0);
    prev_point *= inradius; next_point *= inradius;
    float angle_between = -3.141592653589;
    int total_points = 1;

    for (int i=0; i<indetail; ++i) {
        total_points = total_points*2+1;
        angle_between /= 2;
    }

    for (int i=0; i<=total_points; ++i) {
        //rotate the leading point
        next_point.rotate2Dr(angle_between);
        //add leading point
        vec_vert->push_back( next_point.x + inpos.x ); vec_vert->push_back(next_point.y + inpos.y ); vec_vert->push_back( next_point.z + inpos.z );
        vec_color->push_back(incolor.x); vec_color->push_back(incolor.y); vec_color->push_back(incolor.z); vec_color->push_back(incolor.w);
        //printf("%f, %f\n", next_point.x, next_point.y);
        //center
        vec_vert->push_back( inpos.x ); vec_vert->push_back( inpos.y ); vec_vert->push_back( inpos.z );
        vec_color->push_back(incolor.x); vec_color->push_back(incolor.y); vec_color->push_back(incolor.z); vec_color->push_back(incolor.w);
        //add trailing point
        vec_vert->push_back( prev_point.x + inpos.x ); vec_vert->push_back(prev_point.y + inpos.y ); vec_vert->push_back( prev_point.z + inpos.z );
        vec_color->push_back(incolor.x); vec_color->push_back(incolor.y); vec_color->push_back(incolor.z); vec_color->push_back(incolor.w);
        //rotate the trailing point
        prev_point.rotate2Dr(angle_between);
    }

    return total_points;
}
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
    quaternion(const vec3f &in) { d_x=in.x; d_y=in.y; d_z=in.z; d_w=0.0; }
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

    vec3f mult(const vec3f &in) { quaternion temp(in); return (((*this)*temp)*((*this).conjugate())).toVec3f(); }

    double length() { return sqrt(d_x*d_x + d_y*d_y + d_z*d_z + d_w*d_w); }
    double length_sq() { return (d_x*d_x + d_y*d_y + d_z*d_z + d_w*d_w); }

    void normalize() { double R; R = sqrt(d_w*d_w + d_x*d_x + d_y*d_y + d_z*d_z);
                       d_w /= R; d_x /= R; d_y /= R; d_z /= R; }

    quaternion conjugate() { return quaternion(-d_x, -d_y, -d_z, d_w); }

    //vec3f toVec3f() { return vec3f( (d_x * (d_z + d_z))+(d_w * (d_y + d_y)),
    //                                (d_y * (d_z + d_z))-(d_w * (d_x + d_x)),
    //                                1 - ((d_x * (d_x + d_x))+(d_y * (d_y + d_y))) ); }

    vec3f toVec3f() { return vec3f( d_x, d_y, d_z ); }

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

    void createFromAxisAngle(const vec3f &in, const float &degrees) { createFromAxisAngle(in.x, in.y, in.z, degrees); }
    void createFromAxisAngle(const float &xin, const float &yin, const float &zin, const float &degrees) {
                                float result;
                                result = sin( degrees * 0.00872664625f );
                                // Calcualte the w value by cos( theta / 2 )
                                d_w = cos( degrees * 0.00872664625f );
                                // Calculate the x, y and z of the quaternion
                                d_x = xin * result;
                                d_y = yin * result;
                                d_z = zin * result; }

};


#endif



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
/*
    bool sphereIntersect(const vec3f &startp, const float &radius, float &dist) {

                    normalize();
                    //printf("%1.1f ", dot(spherep-startp));
                    static float b,q;
                    b = startp.dot(*this);

                    if ( b > 0 )
                        //no intersections, we are behind camera
                        return 0;

                    q = b*b - startp.length_sq() + radius*radius;
                    if ( q < 0 )
                        //no intersections
                        return 0;
                    else if ( q == 0 ) {
                        //we only have one intersection point
                        dist = b;
                        return 1;
                    } else if ( q > 0 ) {
                        //two intersections, only return the closest
                        if ( b - q >= 0) dist = b - q;
                        else dist = b + q;
                        return 1;
                    }
                    //shouldn't be here
                    return 0;
*/
/*
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
void drawBox(float x, float y, float z, float s, bool tex) {
    //topperspectiveProjectionMatrix
    if (tex) glTexCoord2f(0,0); glNormal3f(0,1,0); glVertex3f(x-s,y+s,z+s);
    if (tex) glTexCoord2f(0,1); glNormal3f(0,1,0); glVertex3f(x-s,y+s,z-s);
    if (tex) glTexCoord2f(1,1); glNormal3f(0,1,0); glVertex3f(x+s,y+s,z-s);
    if (tex) glTexCoord2f(1,0); glNormal3f(0,1,0); glVertex3f(x+s,y+s,z+s);
    //bottom
    if (tex) glTexCoord2f(1,0); glNormal3f(0,-1,0); glVertex3f(x+s,y-s,z+s);
    if (tex) glTexCoord2f(1,1); glNormal3f(0,-1,0); glVertex3f(x+s,y-s,z-s);
    if (tex) glTexCoord2f(0,1); glNormal3f(0,-1,0); glVertex3f(x-s,y-s,z-s);
    if (tex) glTexCoord2f(0,0); glNormal3f(0,-1,0); glVertex3f(x-s,y-s,z+s);
    //left
    if (tex) glTexCoord2f(1,0); glNormal3f(-1,0,0); glVertex3f(x-s,y-s,z+s);
    if (tex) glTexCoord2f(0,0); glNormal3f(-1,0,0); glVertex3f(x-s,y-s,z-s);
    if (tex) glTexCoord2f(0,1); glNormal3f(-1,0,0); glVertex3f(x-s,y+s,z-s);
    if (tex) glTexCoord2f(1,1); glNormal3f(-1,0,0); glVertex3f(x-s,y+s,z+s);
    //right
    if (tex) glTexCoord2f(1,1); glNormal3f(1,0,0); glVertex3f(x+s,y+s,z+s);
    if (tex) glTexCoord2f(0,1); glNormal3f(1,0,0); glVertex3f(x+s,y+s,z-s);
    if (tex) glTexCoord2f(0,0); glNormal3f(1,0,0); glVertex3f(x+s,y-s,z-s);
    if (tex) glTexCoord2f(1,0); glNormal3f(1,0,0); glVertex3f(x+s,y-s,z+s);
    //front
    if (tex) glTexCoord2f(1,0); glNormal3f(0,0,-1); glVertex3f(x+s,y-s,z-s);
    if (tex) glTexCoord2f(1,1); glNormal3f(0,0,-1); glVertex3f(x+s,y+s,z-s);
    if (tex) glTexCoord2f(0,1); glNormal3f(0,0,-1); glVertex3f(x-s,y+s,z-s);
    if (tex) glTexCoord2f(0,0); glNormal3f(0,0,-1); glVertex3f(x-s,y-s,z-s);
    //back
    if (tex) glTexCoord2f(0,0); glNormal3f(0,0,1); glVertex3f(x-s,y-s,z+s);
    if (tex) glTexCoord2f(0,1); glNormal3f(0,0,1); glVertex3f(x-s,y+s,z+s);
    if (tex) glTexCoord2f(1,1); glNormal3f(0,0,1); glVertex3f(x+s,y+s,z+s);
    if (tex) glTexCoord2f(1,0); glNormal3f(0,0,1); glVertex3f(x+s,y-s,z+s);
}
*/
