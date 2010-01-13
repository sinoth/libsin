#ifndef CAMERA_H
#define CAMERA_H

#include <stdio.h>
#include <GL/glfw.h>
#include <math.h>

#include "sinprim.h"

#define ANG2RAD (3.14159265358979323846/180.0)

//////////////////////////////////////////////////////////////////////////////////
// frustum code courtesy of:
// http://www.lighthouse3d.com/opengl/viewfrustum/index.php?rimp
//////////////////////////////////////////////////////////////////////////////////
// Quaternion camera code from Vic Hollis:
// http://nehe.gamedev.net/data/lessons/lesson.asp?lesson=Quaternion_Camera_Class
//////////////////////////////////////////////////////////////////////////////////

class sinCamera {

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
    vec3f p_arc_center;
    vec3f v_direction;
    vec3f v_strafe_direction;
    vec3f v_up;

    //arcball-esque vars
    vec3f arc_center;
    //vec3f arc_facing;
    float arc_radius;
    quaternion q_arc_rotation;

    //so we can translate mouse coords into world coords
    float f_window_width;
    float f_window_height;

    //frustum funstuff
    float f_fov_angle;
    float f_nearP;
    float f_farP;
    float f_ratio;
    float f_near_width;
    float f_near_height;
    float f_far_width;
    float f_far_height;
    float f_tang;
    float f_sphereFactorX;
    float f_sphereFactorY;


public:

    //for testing
    vec3f arc_facing;

    sinCamera() {
            f_heading_degrees = 0;
            f_pitch_degrees = 0;
            f_max_heading_rate = 5.0f;
            f_max_pitch_rate = 5.0f;
            f_max_pitch = 85.0f;
            f_max_velocity = 5.0f;
            f_max_strafe_velocity = 5.0f;
            f_velocity = 0.0f;
            f_strafe_velocity = 0.0f;
            p_position.set(0,0,0);

            f_window_width = 0;
            f_window_height = 0;

            for ( int i = 0; i < 15; i++ ) {
                af_Matrix_rot[i] = 0.0f;
                af_Matrix_pos[i] = 0.0f;
            }
            af_Matrix_rot[15] = 1.0f;
            af_Matrix_pos[15] = 1.0f;

            recalculateAngle();
        }


    float getMaxPitch() { return f_max_pitch; }
    void setMaxPitch(const float &degrees) { f_max_pitch = degrees; }
    void setVelocity(const float &velocity) { f_velocity = velocity; }
    void setStrafeVelocity(const float &velocity) { f_strafe_velocity = velocity; }
    void setPitch(const float &degrees) { f_pitch_degrees = degrees; }
    void setHeading(const float &degrees) { f_heading_degrees = degrees; }
    void setWindowWH(const float &inw, const float &inh) { f_window_width = inw; f_window_height = inh; }


///////////////////////////
    void setPerspective() {
            // Let OpenGL set our new prespective on the world!
            glMultMatrixf(af_Matrix_rot);
            // Translate to our new position.
            glTranslatef(-p_position.x, -p_position.y, -p_position.z);
        }

/////////////////////////////////////
    void changePitch(const float &degrees) {
            if(fabs(degrees) < f_max_pitch_rate)
            {
                // Our pitch is less than the max pitch rate that we
                // defined so lets increment it.
                f_pitch_degrees += degrees;
            } else {
                // Our pitch is greater than the max pitch rate that
                // we defined so we can only increment pitch by max
                if(degrees < 0) { f_pitch_degrees -= f_max_pitch_rate; }
                else { f_pitch_degrees += f_max_pitch_rate; }
            }
            // clamp pitch between -360 and 360
            if(f_pitch_degrees > 360.0f) { f_pitch_degrees -= 360.0f; }
            else if(f_pitch_degrees < -360.0f) { f_pitch_degrees += 360.0f; }

            if ( f_pitch_degrees > f_max_pitch ) { f_pitch_degrees = f_max_pitch; }
            else if ( f_pitch_degrees < -f_max_pitch ) { f_pitch_degrees = -f_max_pitch; }
        }

////////////////////////////////////////
    void changeHeading(const float &degrees ) {
            if(fabs(degrees) < f_max_heading_rate) {
                f_heading_degrees += degrees;
            } else {
                if ( degrees < 0 )
                    f_heading_degrees -= f_max_pitch_rate;
                else
                    f_heading_degrees += f_max_pitch_rate;
            }
            // clamp heading between -360 and 360
            if(f_heading_degrees > 360.0f) { f_heading_degrees -= 360.0f; }
            else if(f_heading_degrees < 0.0f) { f_heading_degrees += 360.0f; }
        }

///////////////////////////////////////////
    void changeVelocity(const float &vel) {
            if(fabs(vel) < f_max_velocity) {
                f_velocity += vel;
            } else {
                if (vel < 0) { f_velocity -= -f_max_velocity; }
                else { f_velocity += f_max_velocity; }
            }
        }

/////////////////////////////////////////////////
    void changeStrafeVelocity(const float &vel) {
            if(fabs(vel) < f_max_strafe_velocity) {
                f_strafe_velocity += vel;
            } else {
                if(vel < 0) { f_strafe_velocity -= -f_max_strafe_velocity; }
                else { f_strafe_velocity += f_max_strafe_velocity; }
            }
        }

///////////////////////////
    void updatePosition() {
            static vec3f v_temp_dir, v_temp_strafe;
            // Scale the direction by our speed.
            v_temp_dir = v_direction * f_velocity;
            v_temp_strafe = v_strafe_direction * f_strafe_velocity;
            // Increment our position by the vector
            p_position.x += v_temp_dir.x;
            p_position.y += v_temp_dir.y;
            p_position.z += v_temp_dir.z;
            p_position.x += v_temp_strafe.x;
            p_position.y += v_temp_strafe.y;
            p_position.z += v_temp_strafe.z;
            //af_Matrix_rot[12] = -p_position.x;
            //af_Matrix_rot[13] = -p_position.z;
            //af_Matrix_rot[14] = -p_position.y;
            //af_Matrix_rot[15] = 1;
        }

////////////////////////////////
    void setPosition(const vec3f &in) {
            p_position.x = in.x;
            p_position.y = in.y;
            p_position.z = in.z; }

////////////////////////////////////////////////////////////////////////////
    void setPosition(const float &inx, const float &iny, const float &inz) {
            p_position.x = inx;
            p_position.y = iny;
            p_position.z = inz; }

//////////////////
    void reset() {
            p_position.set(0,0,0);
            f_velocity = 0.0f;
            f_strafe_velocity = 0.0f;
            f_pitch_degrees = 0.0f;
            f_heading_degrees = 0.0f;
            recalculateAngle();
        }

/////////////////////////////
    void recalculateAngle() {
            // Make the Quaternions that will represent our rotations
            q_pitch.createFromAxisAngle(1.0f, 0.0f, 0.0f, f_pitch_degrees);
            q_heading.createFromAxisAngle(0.0f, 1.0f, 0.0f, f_heading_degrees);

            (q_heading * q_pitch).toMatrix(af_Matrix_rot);
            (q_pitch * q_heading).toMatrix(af_Matrix_pos);

            v_direction.x = af_Matrix_pos[8];
            v_direction.y = af_Matrix_pos[9];
            v_direction.z = -af_Matrix_pos[10];
            v_strafe_direction.x = af_Matrix_pos[0];
            v_strafe_direction.y = af_Matrix_pos[1];
            v_strafe_direction.z = -af_Matrix_pos[2];
            v_up.x = af_Matrix_pos[4];
            v_up.y = af_Matrix_pos[5];
            v_up.z = -af_Matrix_pos[6];
        }

////////////////////////////////////////////////////////////
	int pointInView( const vec3f &p, const float &radius ) {
            static float d1,d2,az,ax,ay,zz1,zz2;
            static int result;
            static vec3f v;

            result = 2;
            v = p - p_position;

            az = v.dot(v_direction);
            if (az > f_farP + radius || az < f_nearP-radius)
                return(0);

            ax = v.dot(v_strafe_direction);
            zz1 = az * f_tang * f_ratio;
            d1 = f_sphereFactorX * radius;
            if (ax > zz1+d1 || ax < -zz1-d1)
                return(0);

            ay = v.dot(v_up);
            zz2 = az * f_tang;
            d2 = f_sphereFactorY * radius;
            if (ay > zz2+d2 || ay < -zz2-d2)
                return(0);

            if (az > f_farP - radius || az < f_nearP+radius)
                result = 1;
            if (ay > zz2-d2 || ay < -zz2+d2)
                result = 1;
            if (ax > zz1-d1 || ax < -zz1+d1)
                result = 1;

            return(result);
        }

////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void setInternals(const float &inangle, const float &inratio, const float &innear, const float &infar) {
            // store the information
            f_ratio = inratio;
            f_nearP = innear;
            f_farP  = infar;
            f_fov_angle = inangle * ANG2RAD;
            // compute width and height of the frustum
            f_tang = (float)tan(f_fov_angle);
            f_near_height = 2 * innear * (float)tan(f_fov_angle/2);
            f_near_width = f_near_height * inratio;
            f_far_height = 2 * infar * (float)tan(f_fov_angle/2);
            f_far_width = f_far_height * inratio;
            //printf("near w/h: %f, %f  far w/h: %f, %f\n", f_near_width, f_near_height, f_far_width, f_far_height);
            // compute sphere factor for picking
            f_sphereFactorY = 1.0/cos(f_fov_angle);
            f_sphereFactorX = 1.0/cos(atan(f_tang*inratio));
        }

///////////////////////////////////////////////////////////////////////////////////////////
    void getMouseRay(const float &inx, const float &iny, vec3f &out_pos, vec3f &out_dir ) {

            out_pos.set( inx/f_window_width*f_near_width - f_near_width/2,
                         -iny/f_window_height*f_near_height + f_near_height/2,
                         -f_nearP );
            //printf("mouseRay point: %f, %f, %f\n", out_pos.x, out_pos.y, out_pos.z );
            //printf("x: %f, y: %f\n", inx, iny);
            //printf("winx: %f, winy: %f\n", f_window_width, f_window_height);
            //printf("nearw,h: %f,%f : far: %f,%f\n", f_near_width, f_near_height, f_far_width, f_far_height);

            out_pos.inv_mult_by_matrix(af_Matrix_rot);
            out_pos += p_position;
            //printf("mouseRay point proj: %f, %f, %f\n", out_pos.x, out_pos.y, out_pos.z );


            vec3f temp_far_pos( inx/f_window_width*f_far_width - f_far_width/2,
                                -iny/f_window_height*f_far_height + f_far_height/2,
                                -f_farP );

            temp_far_pos.inv_mult_by_matrix(af_Matrix_rot);
            temp_far_pos += p_position;

            out_dir = (temp_far_pos - out_pos);
            out_dir.normalize();

            //printf("pos: %.3f, %.3f, %.3f, fpos: %.3f, %.3f, %.3f, dir: %.3f, %.3f, %.3f\n", out_pos.x, out_pos.y, out_pos.z, temp_far_pos.x, temp_far_pos.y, temp_far_pos.z, out_dir.x, out_dir.y, out_dir.z );


        }

///////////////////////////////////////////////
// arcball-esque stuff ////////////////////////
///////////////////////////////////////////////


    void arcZoom(const float &in) { arc_radius += in; arcRecalculate(); }
    void arcSetRadius(const float &in) { arc_radius = in; }
    void arcSetCenter(const vec3f &in) { arc_center = in; }
    void arcSetFacing(const vec3f &in) { arc_facing = in; }
    void arcSetRotation(const quaternion &in) { q_arc_rotation = in; }

/////////////////////////////////////////
    void arcSpinMouseX(const float &in) {
                static quaternion temp;
                static vec3f up(0,1,0);
                temp.createFromAxisAngle( up, in );
                q_arc_rotation *= temp;
                q_arc_rotation.normalize();
                arcRecalculate(); }

/////////////////////////////////////////
    void arcSpinMouseY(const float &in) {
                static quaternion temp;
                static vec3f right(-1,0,0);
                temp.createFromAxisAngle( right, in );
                q_arc_rotation *= temp;
                q_arc_rotation.normalize();
                arcRecalculate(); }

///////////////////////////
    void arcRecalculate() {
                q_arc_rotation.toMatrix(af_Matrix_rot);

                arc_facing.x = -af_Matrix_rot[2];
                arc_facing.y = -af_Matrix_rot[6];
                arc_facing.z = -af_Matrix_rot[10];

                printf("facing: %f, %f, %f\n", arc_facing.x, arc_facing.y, arc_facing.z );

                p_position = (-arc_facing * arc_radius + arc_center);
            }

};



#endif





/*

    //void updatePositionXPlane(float,float);
    //void updatePositionYPlane(float,float);

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
