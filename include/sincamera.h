#ifndef CAMERA_H
#define CAMERA_H

#include "sinprim.h"

#define ANG2RAD (3.14159265358979323846/360.0)

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
    void setPosition(vec3f);
    void setPosition(float,float,float);
    void setPitch(float);
    void setHeading(float);
    void reset();
    //void updatePositionXPlane(float,float);
    //void updatePositionYPlane(float,float);
    void recalculateAngle();
	int pointInView(vec3f &p, float );
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
    vec3f p_position;
    vec3f v_direction;
    vec3f v_strafe_direction;
    vec3f v_up;

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





/*
void sinCamera::updatePositionXPlane(float mouse_inx, float mouse_iny) {

	// Create a matrix from the pitch Quaternion and get the j vector
	// for our direction.
	//q_pitch.createMatrix(af_Matrix);
	//v_direction.y = af_Matrix[9];

	// Combine the heading and pitch rotations and make a matrix to get
	// the i and j vectors for our direction. ???
	q_temp = q_heading;
	q_temp.createMatrix(af_Matrix_pos);
	v_direction.x = af_Matrix_pos[8];
	v_direction.y = af_Matrix_pos[9];
    v_direction.z = af_Matrix_pos[10];
	v_strafe_direction.x = af_Matrix_pos[0];
	v_strafe_direction.y = af_Matrix_pos[1];
    v_strafe_direction.z = af_Matrix_pos[2];

    //printf("%f,%f,%f\n",v_direction.x,v_direction.y,v_direction.z);

    // Scale the direction by our speed.
	v_direction *= -mouse_iny;
	v_strafe_direction *= mouse_inx;

	// Increment our position by the vector
	p_position.x += v_direction.x;
	//p_position.y += v_direction.y;
	p_position.z += v_direction.z;
    p_position.x += v_strafe_direction.x;
	//p_position.y += v_strafe_direction.y;
	p_position.z += v_strafe_direction.z;

}
void sinCamera::updatePositionYPlane(float mouse_inx, float mouse_iny) {

	// Create a matrix from the pitch Quaternion and get the j vector
	// for our direction.
	//q_pitch.createMatrix(af_Matrix);
	//v_direction.y = af_Matrix[9];

	// Combine the heading and pitch rotations and make a matrix to get
	// the i and j vectors for our direction. ???
	q_temp = q_up;
	q_temp.createMatrix(af_Matrix_pos);
	v_direction.x = af_Matrix_pos[8];
	v_direction.y = af_Matrix_pos[9];
    v_direction.z = af_Matrix_pos[10];
	v_strafe_direction.x = af_Matrix_pos[0];
	v_strafe_direction.y = af_Matrix_pos[1];
    v_strafe_direction.z = af_Matrix_pos[2];

    //printf("%f,%f,%f\n",v_direction.x,v_direction.y,v_direction.z);

    // Scale the direction by our speed.
	v_direction *= -mouse_iny;
	v_strafe_direction *= mouse_inx;

	// Increment our position by the vector
	//p_position.x += v_direction.x;
	p_position.y += v_direction.y;
	//p_position.z += v_direction.z;
    //p_position.x += v_strafe_direction.x;
	p_position.y += v_strafe_direction.y;
	//p_position.z += v_strafe_direction.z;

}
*/
