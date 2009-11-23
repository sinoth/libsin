
#include "sinprim.h"
#include <math.h>


///////////////////////////////////////
// point shizzle
///////////////////////////////////////
//

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




//////////////////////////////////////////
// Quaternion camera code from Vic Hollis
// http://nehe.gamedev.net/data/lessons/lesson.asp?lesson=Quaternion_Camera_Class
//////////////////////////////////////////////////////////////////////////////////
////////////////
// quaternion shizzle
//

quaternion quaternion::operator *(quaternion in)
{
	static quaternion r;

	r.d_w = d_w*in.d_w - d_x*in.d_x - d_y*in.d_y - d_z*in.d_z;
	r.d_x = d_w*in.d_x + d_x*in.d_w + d_y*in.d_z - d_z*in.d_y;
	r.d_y = d_w*in.d_y + d_y*in.d_w + d_z*in.d_x - d_x*in.d_z;
	r.d_z = d_w*in.d_z + d_z*in.d_w + d_x*in.d_y - d_y*in.d_x;

	return(r);
}

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


void quaternion::createMatrix(float *pMatrix)
{
	// Make sure the matrix has allocated memory to store the rotation data
	if(!pMatrix) return;

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
