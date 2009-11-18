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
    void changePitch(float);
    void changeHeading(float);
    void changeVelocity(float);
    void changeStrafeVelocity(float);
    void setVelocity(float);
    void setStrafeVelocity(float);
    void updatePosition();
    void setPosition(vector);
    void setPosition(float,float,float);
    void setPitch(float);
    void setHeading(float);
    void reset();
    //void updatePositionXPlane(float,float);
    //void updatePositionYPlane(float,float);
    void recalculateAngle();
	int pointInView(point &p, float );
    void setInternals(float, float, float, float);
    float getMaxPitch();
    void setMaxPitch(const float&);


private:
    float f_pitch_degrees;
    float f_max_pitch_rate;
    float f_max_pitch;
    float f_heading_degrees;
    float f_max_heading_rate;
    float f_velocity;
    float f_max_velocity;
    float f_strafe_velocity;
    float f_max_strafe_velocity;

    float af_Matrix_rot[16];
    float af_Matrix_pos[16];

    quaternion q_pitch;
    quaternion q_heading;
    point p_position;
    vector v_direction;
    vector v_strafe_direction;
    vector v_up;

    //frustum funstuff
    float f_fov_angle;
    float f_nearP;
    float f_farP;
    float f_ratio;
    float f_width;
    float f_height;
    float f_tang;
    float f_sphereFactorX;
    float f_sphereFactorY;

};



#endif //\/ CAMERA_H
