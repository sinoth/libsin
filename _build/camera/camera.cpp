#include <GL/glfw.h>
#include <math.h>

#include "sincamera.h"

//////////////////////////////////////////
// Quaternion camera code from Vic Hollis
// http://nehe.gamedev.net/data/lessons/lesson.asp?lesson=Quaternion_Camera_Class
//////////////////////////////////////////////////////////////////////////////////

sinCamera::sinCamera() {
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

    for ( int i = 0; i < 15; i++ ) {
        af_Matrix_rot[i] = 0.0f;
        af_Matrix_pos[i] = 0.0f;
    }
    af_Matrix_rot[15] = 1.0f;
    af_Matrix_pos[15] = 1.0f;

    recalculateAngle();

}

float sinCamera::getMaxPitch() { return f_max_pitch; }
void sinCamera::setMaxPitch(const float &in) { f_max_pitch = in; }

void sinCamera::reset() {
    p_position.set(0,0,0);
    f_velocity = 0.0f;
    f_strafe_velocity = 0.0f;
    f_pitch_degrees = 0.0f;
    f_heading_degrees = 0.0f;
    recalculateAngle();
}

void sinCamera::setPosition( vector in ) {
    p_position.x = in.x;
    p_position.y = in.y;
    p_position.z = in.z; }

void sinCamera::setPosition( float inx, float iny, float inz ) {
    p_position.x = inx;
    p_position.y = iny;
    p_position.z = inz; }

void sinCamera::setPerspective()
{

    // Let OpenGL set our new prespective on the world!
    glMultMatrixf(af_Matrix_rot);

    // Translate to our new position.
    glTranslatef(-p_position.x, -p_position.y, -p_position.z);

}


void sinCamera::updatePosition() {

	static vector v_temp_dir, v_temp_strafe;

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


void sinCamera::setPitch(float degrees) {
    f_pitch_degrees = degrees;
}

void sinCamera::setHeading(float degrees) {
    f_heading_degrees = degrees;
}

void sinCamera::changePitch(float degrees)
{

	if(fabs(degrees) < f_max_pitch_rate)
	{
		// Our pitch is less than the max pitch rate that we
		// defined so lets increment it.
		f_pitch_degrees += degrees;
	}
	else
	{
		// Our pitch is greater than the max pitch rate that
		// we defined so we can only increment our pitch by the
		// maximum allowed value.
		if(degrees < 0)
		{
			// We are pitching down so decrement
			f_pitch_degrees -= f_max_pitch_rate;
		}
		else
		{
			// We are pitching up so increment
			f_pitch_degrees += f_max_pitch_rate;
		}
	}

	// We don't want our pitch to run away from us. Although it
	// really doesn't matter I prefer to have my pitch degrees
	// within the range of -360.0f to 360.0f
	if(f_pitch_degrees > 360.0f)
	{
		f_pitch_degrees -= 360.0f;
	}
	else if(f_pitch_degrees < -360.0f)
	{
		f_pitch_degrees += 360.0f;
	}

	if ( f_pitch_degrees > f_max_pitch ) { f_pitch_degrees = f_max_pitch; }
	else if ( f_pitch_degrees < -f_max_pitch ) { f_pitch_degrees = -f_max_pitch; }

}

void sinCamera::changeHeading(float degrees)
{
	if(fabs(degrees) < f_max_heading_rate)
	{
        f_heading_degrees += degrees;
	}
	else
	{
        if ( degrees < 0 )
            f_heading_degrees -= f_max_pitch_rate;
        else
            f_heading_degrees += f_max_pitch_rate;
	}

	// We don't want our heading to run away from us either. Although it
	// really doesn't matter I prefer to have my heading degrees
	// within the range of -360.0f to 360.0f
	if(f_heading_degrees > 360.0f)
	{
		f_heading_degrees -= 360.0f;
	}
	else if(f_heading_degrees < 0.0f)
	{
		f_heading_degrees += 360.0f;
	}
}

void sinCamera::recalculateAngle() {

	// Make the Quaternions that will represent our rotations
	q_pitch.createFromAxisAngle(1.0f, 0.0f, 0.0f, f_pitch_degrees);
	q_heading.createFromAxisAngle(0.0f, 1.0f, 0.0f, f_heading_degrees);

	(q_heading * q_pitch).createMatrix(af_Matrix_rot);
	(q_pitch * q_heading).createMatrix(af_Matrix_pos);

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

void sinCamera::changeVelocity(float vel)
{
	if(fabs(vel) < f_max_velocity)
	{
		// Our velocity is less than the max velocity increment that we
		// defined so lets increment it.
		f_velocity += vel;
	}
	else
	{
		// Our velocity is greater than the max velocity increment that
		// we defined so we can only increment our velocity by the
		// maximum allowed value.
		if(vel < 0)
		{
			// We are slowing down so decrement
			f_velocity -= -f_max_velocity;
		}
		else
		{
			// We are speeding up so increment
			f_velocity += f_max_velocity;
		}
	}
}

void sinCamera::changeStrafeVelocity(float vel)
{
	if(fabs(vel) < f_max_strafe_velocity)
	{
		// Our velocity is less than the max velocity increment that we
		// defined so lets increment it.
		f_strafe_velocity += vel;
	}
	else
	{
		// Our velocity is greater than the max velocity increment that
		// we defined so we can only increment our velocity by the
		// maximum allowed value.
		if(vel < 0)
		{
			// We are slowing down so decrement
			f_strafe_velocity -= -f_max_strafe_velocity;
		}
		else
		{
			// We are speeding up so increment
			f_strafe_velocity += f_max_strafe_velocity;
		}
	}
}

void sinCamera::setVelocity(float vel) { f_velocity = vel; }
void sinCamera::setStrafeVelocity(float vel) { f_strafe_velocity = vel; }


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

void sinCamera::setInternals(float inangle, float inratio, float innear, float infar) {

	// store the information
	f_ratio = inratio;
	f_nearP = innear;
	f_farP  = infar;
	f_fov_angle = inangle * ANG2RAD;

	// compute width and height of te near section
	f_tang = (float)tan(f_fov_angle) ;
	f_height = innear * f_tang;
	f_width = f_height * inratio;

	f_sphereFactorY = 1.0/cos(f_fov_angle);
	f_sphereFactorX = 1.0/cos(atan(f_tang*inratio));
}



int sinCamera::pointInView( point &p, float radius ) {

	static float d1,d2;
	static float az,ax,ay,zz1,zz2;
	static int result;
    static vector v;

	result = 2;
	v = p.subToVector(p_position);

	az = v.dotproduct(v_direction);
	if (az > f_farP + radius || az < f_nearP-radius)
		return(0);

	ax = v.dotproduct(v_strafe_direction);
	zz1 = az * f_tang * f_ratio;
	d1 = f_sphereFactorX * radius;
	if (ax > zz1+d1 || ax < -zz1-d1)
		return(0);

	ay = v.dotproduct(v_up);
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
