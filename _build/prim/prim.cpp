
#include "prim.h"
#include <math.h>
#include <stdio.h>


///////////////////////////////////////
// colour shizzle
///////////////////////////////////////
//

colour::colour(float inr, float ing, float inb, float ina) {
    r=inr; g=ing; b=inb; a=ina; }

colour::colour() { r=0; g=0; b=0; a=0; }

void colour::set( float inr, float ing, float inb, float ina ) {
    r=inr; g=ing; b=inb; a=ina; }

void colour::draw() { glColor4f(r,g,b,a); }



///////////////////////////////////////
// point shizzle
///////////////////////////////////////
//

point::point(float inx, float iny, float inz) {
    x=inx; y=iny; z=inz; }

point::point(int inx, int iny, int inz) {
    x=inx; y=iny; z=inz; }

point::point() {
    x=0; y=0; z=0; }

void point::draw() {
    glVertex3f(x,y,z); }

void point::set( float inx, float iny, float inz) {
    x=inx; y=iny; z=inz; }

float point::distance( point in ) {
    //return sqrt((x-in.x)*(x-in.x) + (y-in.y)*(y-in.y) + (z-in.z)*(z-in.z));
    return ((x-in.x)*(x-in.x) + (y-in.y)*(y-in.y) + (z-in.z)*(z-in.z));
    }

point point::operator +(vector in) {
    static point r;
    r.x = in.x + x;
    r.y = in.y + y;
    r.z = in.z + z;
    return r; }


vector point::subToVector(point in) {
    static vector r;
    r.x = x - in.x;
    r.y = y - in.y;
    r.z = z - in.z;
    return r; }


void point::mult_by_matrix( GLfloat *in )
{
    static float x2,y2,z2;

    x2 = x*in[0] + y*in[4] + z*in[8];
    y2 = x*in[1] + y*in[5] + z*in[9];
    z2 = x*in[2] + y*in[6] + z*in[10];

    x=x2; y=y2; z=z2;
}

point point::operator -(point in) {
    static point r;
    r.x = x - in.x;
    r.y = y - in.y;
    r.z = z - in.z;
    return r; }

point point::operator +(point in) {
    static point r;
    r.x = x + in.x;
    r.y = y + in.y;
    r.z = z + in.z;
    return r; }

point point::operator /(point in) {
    static point r;
    r.x = x / in.x;
    r.y = y / in.y;
    r.z = z / in.z;
    return r; }

void vector::print(char *in) { printf("--- [%s] %+.5f, %+.5f, %+.5f\n", in, x, y, z ); }

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

float point::ray_point_distance( vector ray_dir, point check ) {

	static vector t;
	static float dot;
	static point best_point;

	t.x = check.x - x;
	t.y = check.y - y;
	t.z = check.z - z;

	dot = t.dotproduct(ray_dir);

	if ( dot < 0.0 ) {
	    dot = 0.0;
	    best_point = *this;
	} else {
	    best_point = *this + (ray_dir * dot);
	}

    //printf("ray_start: %f,%f,%f\n",ray_start.x,ray_start.y,ray_start.z);
    //printf("ray_dir: %f,%f,%f\n---\n",ray_dir.x,ray_dir.y,ray_dir.z);
    //printf("line: %f,%f,%f\n",best_point.x,best_point.y,best_point.z);
    //printf("check: %f,%f,%f\n\n",check.x,check.y,check.z);
	return fabs(best_point.distance(check));
}

///////////////////////////////////////
// vector shizzle
///////////////////////////////////////
//

vector::vector( double xin, double yin, double zin )
    { x = xin; y = yin; z = zin; }

vector::vector() { x=0; y=0; z=0; }

void vector::subtract( vector in )
    { x -= in.x; y -= in.y; z -= in.z; }

void vector::normalize() {
	static double ln;
	ln = sqrt( x*x + y*y + z*z );
	if (ln == 0) return;

	x = x / ln;
	y = y / ln;
	z = z / ln; }

void vector::crossproduct( vector in ) {
	static vector tempv;

	tempv.x = y * in.z - z * in.y;
	tempv.y = z * in.x - x * in.z;
	tempv.z = x * in.y - y * in.x;

	x=tempv.x; y=tempv.y; z=tempv.z;
	}

float vector::dotproduct( vector in )
    { return x*in.x + y*in.y + z*in.z; }

vector vector::operator *(vector in)
{ static vector r;
  r.x = y*in.z - z*in.y;
  r.y = z*in.x - x*in.z;
  r.z = x*in.y - y*in.x;
  return(r); }

vector vector::operator *(float in)
{ static vector r;
  r.x = x * in;
  r.y = y * in;
  r.z = z * in;
  return(r); }

void vector::operator *=(float in)
{ x *= in; y *= in; z *= in; }

void vector::mult_by_matrix( GLfloat *in )
{
    static float x2,y2,z2;
/*
    x2 = x*in[0] + y*in[4] + z*in[8];
    printf("\n%+.5f = %+.5f*%+.5f + %+.5f*%+.5f + %+.5f*%+.5f\n",x2 , x,in[0] , y,in[4] , z,in[8]);
    y2 = x*in[1] + y*in[5] + z*in[9];
    printf("%+.5f = %+.5f*%+.5f + %+.5f*%+.5f + %+.5f*%+.5f\n",y2 , x,in[1] , y,in[5] , z,in[9]);
    z2 = x*in[2] + y*in[6] + z*in[10];
    printf("%+.5f = %+.5f*%+.5f + %+.5f*%+.5f + %+.5f*%+.5f\n",z2 , x,in[2] , y,in[6] , z,in[10]);
*/

    x2 = x*in[0] + y*in[4] + z*in[8];
    //printf("\n%+.5f = %+.5f*%+.5f + %+.5f*%+.5f + %+.5f*%+.5f\n",x2 , x,in[0] , y,in[4] , z,in[8]);
    y2 = x*in[1] + y*in[5] + z*in[9];
    //printf("%+.5f = %+.5f*%+.5f + %+.5f*%+.5f + %+.5f*%+.5f\n",y2 , x,in[1] , y,in[5] , z,in[9]);
    z2 = x*in[2] + y*in[6] + z*in[10];
    //printf("%+.5f = %+.5f*%+.5f + %+.5f*%+.5f + %+.5f*%+.5f\n",z2 , x,in[2] , y,in[6] , z,in[10]);

    //x=-x2; y=-y2; z=-z2;
    x=x2; y=y2; z=z2;
}

void vector::createFromPoints( point a, point b ) {
    x = b.x - a.x;
    y = b.y - a.y;
    z = b.z - a.z;
}

void vector::set(double xin, double yin, double zin) { x = xin; y = yin; z = zin; }

void vector::rotate(float degrees) {
    float rad = degrees * 0.0174532925;
    float tx = (x * cosf(rad)) - (y * sinf(rad));
    float ty = (x * sinf(rad)) + (y * cosf(rad));
    x = tx;
    y = ty;
}


//////////////////////////////////////////
// Quaternion camera code from Vic Hollis
// http://nehe.gamedev.net/data/lessons/lesson.asp?lesson=Quaternion_Camera_Class
//////////////////////////////////////////////////////////////////////////////////
////////////////
// quaternion shizzle
//

quaternion::quaternion( double xin, double yin, double zin, double win )
    { d_x = xin; d_y = yin; d_z = zin; d_w = win; }

quaternion::quaternion( ) { d_x=0; d_y=0; d_z=0; d_w=1; }

void quaternion::conjugate() { d_x = -d_x; d_y = -d_y; d_z = -d_z; }

double quaternion::length() { return sqrt(d_x*d_x + d_y*d_y + d_z*d_z + d_w*d_w); }

quaternion quaternion::operator *(quaternion in)
{
	static quaternion r;

	r.d_w = d_w*in.d_w - d_x*in.d_x - d_y*in.d_y - d_z*in.d_z;
	r.d_x = d_w*in.d_x + d_x*in.d_w + d_y*in.d_z - d_z*in.d_y;
	r.d_y = d_w*in.d_y + d_y*in.d_w + d_z*in.d_x - d_x*in.d_z;
	r.d_z = d_w*in.d_z + d_z*in.d_w + d_x*in.d_y - d_y*in.d_x;

	return(r);
}

void quaternion::createFromAxisAngle(GLfloat xin, GLfloat yin, GLfloat zin, GLfloat degrees)
{
	//static const GLdouble pi = 3.14159265;

	// First we want to convert the degrees to radians
	// since the angle is assumed to be in radians
	static GLfloat angle;
	static GLfloat result;

	//angle = GLfloat((degrees / 180.0f) * 3.14159265);
	angle = degrees * 0.0174532925;

	// Here we calculate the sin( theta / 2) once for optimization
	result = (GLfloat)sin( angle / 2.0f );

	// Calcualte the w value by cos( theta / 2 )
	d_w = (GLfloat)cos( angle / 2.0f );

	// Calculate the x, y and z of the quaternion
	d_x = GLfloat(xin * result);
	d_y = GLfloat(yin * result);
	d_z = GLfloat(zin * result);
}


void quaternion::createMatrix(GLfloat *pMatrix)
{
	// Make sure the matrix has allocated memory to store the rotation data
	if(!pMatrix) return;
/*
	// First row
	pMatrix[ 0] = 1.0f - 2.0f * ( d_y * d_y + d_z * d_z );
	pMatrix[ 1] = 2.0f * (d_x * d_y + d_z * d_w);
	pMatrix[ 2] = 2.0f * (d_x * d_z - d_y * d_w);
	//pMatrix[ 3] = 0.0f;

	// Second row
	pMatrix[ 4] = 2.0f * ( d_x * d_y - d_z * d_w );
	pMatrix[ 5] = 1.0f - 2.0f * ( d_x * d_x + d_z * d_z );
	pMatrix[ 6] = 2.0f * (d_z * d_y + d_x * d_w );
	//pMatrix[ 7] = 0.0f;

	// Third row
	pMatrix[ 8] = 2.0f * ( d_x * d_z + d_y * d_w );
	pMatrix[ 9] = 2.0f * ( d_y * d_z - d_x * d_w );
	pMatrix[10] = 1.0f - 2.0f * ( d_x * d_x + d_y * d_y );
	//pMatrix[11] = 0.0f;
*/

	// First row
	pMatrix[ 0] = 1.0f - (2.0f * ( d_y * d_y + d_z * d_z ));
	pMatrix[ 1] = 2.0f * (d_x * d_y - d_z * d_w);
	pMatrix[ 2] = 2.0f * (d_x * d_z + d_y * d_w);
	//pMatrix[ 3] = 0.0f;

	// Second row
	pMatrix[ 4] = 2.0f * ( d_x * d_y + d_z * d_w );
	pMatrix[ 5] = 1.0f - (2.0f * ( d_x * d_x + d_z * d_z ));
	pMatrix[ 6] = 2.0f * (d_z * d_y - d_x * d_w );
	//pMatrix[ 7] = 0.0f;

	// Third row
	pMatrix[ 8] = 2.0f * ( d_x * d_z - d_y * d_w );
	pMatrix[ 9] = 2.0f * ( d_y * d_z + d_x * d_w );
	pMatrix[10] = 1.0f - (2.0f * ( d_x * d_x + d_y * d_y ));
	//pMatrix[11] = 0.0f;


	// Fourth row
	//pMatrix[12] = 0;
	//pMatrix[13] = 0;
	//pMatrix[14] = 0;
	//pMatrix[15] = 1.0f;

	// Now pMatrix[] is a 4x4 homogeneous matrix that can be applied to an OpenGL Matrix
}

vector quaternion::toVector() {
    float x2 = d_x + d_x, y2 = d_y + d_y, z2 = d_z + d_z;
    float xx = d_x * x2, xz = d_x * z2;
    float yy = d_y * y2, yz = d_y * z2;
    float wx = d_w * x2, wy = d_w * y2;
    return vector( xz+wy, yz-wx, 1 - (xx+yy) );
}


float* quaternion::toMatrix() {

    float* theMatrix = new float[16];

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

    return theMatrix;
}


void quaternion::normalize() {
    static double R;

    R = sqrt(d_w*d_w + d_x*d_x + d_y*d_y + d_z*d_z);

    d_w /= R;
    d_x /= R;
    d_y /= R;
    d_z /= R;
}

/*
void quaternion::mult(quaternion in) {
  d_x = d_w*in.d_x + d_x*in.d_w + d_y*in.d_z - d_z*in.d_y;
  d_y = d_w*in.d_y - d_x*in.d_z + d_y*in.d_w + d_z*in.d_x;
  d_z = d_w*in.d_z + d_x*in.d_y - d_y*in.d_x + d_z*in.d_w;
  d_w = d_w*in.d_w - d_x*in.d_x - d_y*in.d_y - d_z*in.d_z;
}
*/
