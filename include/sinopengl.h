
#include <GL/glfw.h>

/////////////////////////////////////////////////////////
void glFrustumInfinite( double left, double right,
                        double bottom, double top,
                        double nearval, double ) {
  double x, y, a, b, d;
  x = (2.0 * nearval) / (right - left);
  y = (2.0 * nearval) / (top - bottom);
  a = (right + left) / (right - left);
  b = (top + bottom) / (top - bottom);
  // Infinite view frustum
  //c = -1.0;
  d = -2.0 * nearval;

  double m[] = { x,  0,  0,  0,
                 0,  y,  0,  0,
                 a,  b, -1, -1,
                 0,  0,  d,  0};

  glLoadMatrixd(m);
}


// Replaces gluPerspective. Sets the frustum to perspective mode.
// fovY		- Field of vision in degrees in the y direction
// aspect	- Aspect ratio of the viewport
// zNear	- The near clipping distance
// zFar		- The far clipping distance
void perspectiveGL( GLdouble fovY, GLdouble aspect, GLdouble zNear, GLdouble zFar )
{
	//static const GLdouble pi = 3.1415926535897932384626433832795;
	//static const GLdouble pi = 3.14159265;
    static const GLdouble PI_360 = 0.008726646259; //9716388888888888888889
	static GLdouble fW, fH;

	//fH = tan( fovY / 360.0 * 3.14159 ) * zNear;
	fH = tan( fovY * PI_360 ) * zNear;
	fW = fH * aspect;

    if ( zFar == -1.0f ) //infinite
        glFrustumInfinite( -fW, fW, -fH, fH, zNear, zFar );
    else
        glFrustum( -fW, fW, -fH, fH, zNear, zFar );
}

