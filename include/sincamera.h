#ifndef CAMERA_H
#define CAMERA_H

#include "sinprim.h"

#define ANG2RAD 3.14159265358979323846/360.0

////////////////////////////////////////////////////////////////////////////////
// frustum code courtesy of:
// http://www.lighthouse3d.com/opengl/viewfrustum/index.php?rimp
////////////////////////////////////////////////////////////////////////////////

class sinCamera {
    public:
    sinCamera();
    void setPerspective();
    void changePitch(GLfloat);
    void changeHeading(GLfloat);
    void changeVelocity(GLfloat);
    void changeStrafeVelocity(GLfloat);
    void setVelocity(GLfloat);
    void setStrafeVelocity(GLfloat);
    void updatePosition();
    void setPosition(vector);
    //void updatePositionXPlane(float,float);
    //void updatePositionYPlane(float,float);
    void recalculateAngle();
	int pointInView(point &p, float );
    void setInternals(float, float, float, float);


    GLfloat f_pitch_degrees;
    GLfloat f_max_pitch_rate;
    GLfloat f_max_pitch;
    GLfloat f_heading_degrees;
    GLfloat f_max_heading_rate;
    GLfloat f_velocity;
    GLfloat f_max_velocity;
    GLfloat f_strafe_velocity;
    GLfloat f_max_strafe_velocity;

    GLfloat af_Matrix_rot[16];
    GLfloat af_Matrix_pos[16];

    quaternion q_pitch;
    quaternion q_heading;
    point p_position;
    vector v_direction;
    vector v_strafe_direction;
    vector v_up;

    //frustum funstuff
    GLfloat f_fov_angle;
    GLfloat f_nearP;
    GLfloat f_farP;
    GLfloat f_ratio;
    GLfloat f_width;
    GLfloat f_height;
    GLfloat f_tang;
    GLfloat f_sphereFactorX;
    GLfloat f_sphereFactorY;

};



#endif //\/ CAMERA_H
