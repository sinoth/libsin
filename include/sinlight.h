#ifndef _SINLIGHT_H
#define _SINLIGHT_H

#include <GL/glfw.h>
#include <sinprim.h>

typedef struct sinlight_s
{
    float ambient[4];
    float diffuse[4];
    float specular[4];
    float position[4];

    sinlight_s() {
            ambient[0] = 0.0; ambient[1] = 0.0; ambient[2] = 0.0; ambient[3] = 1.0;
            diffuse[0] = 0.0; diffuse[1] = 0.0; diffuse[2] = 0.0; diffuse[3] = 1.0;
            specular[0] = 0.0; specular[1] = 0.0; specular[2] = 0.0; specular[3] = 1.0;
            position[0] = 0.0; position[1] = 0.0; position[2] = 0.0; position[3] = 0.0;
        }

} sinlight;

////////////////////////////////////////

class sinLighting
{
private:

    sinlight lights[8];
    bool lights_enabled[8];
    bool enabled;
    bool do_ambient;
    float global_ambient_color[4];
    float none[4];

public:

    sinLighting() {
            do_ambient = true;
            enabled = true;
            global_ambient_color[0] = 0.2;
            global_ambient_color[1] = 0.2;
            global_ambient_color[2] = 0.2;
            global_ambient_color[3] = 1.0;
            none[0] = 0.0; none[1] = 0.0; none[2] = 0.0; none[3] = 0.0;
            for (int i=0; i<8; i++) lights_enabled[i] = false;
        }

    void enable() { enabled = true; }
    void disable() { enabled = false; }
    void disableGlobalAmbient() { glLightModelfv(GL_LIGHT_MODEL_AMBIENT, none); }
    void enableGlobalAmbient() { glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient_color); }
    void enableLight(const int &l) { glEnable(GL_LIGHT0+l); lights_enabled[l] = true; }
    void disableLight(const int &l) { glDisable(GL_LIGHT0+l); lights_enabled[l] = false; }

/////////////////
    void init() {
            glEnable(GL_COLOR_MATERIAL);
            glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
            //glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, .001);
            //glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.02f);
            //glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.008f);
        }

///////////////////////
    void doLighting() {
            if ( enabled ) {
                glEnable(GL_LIGHTING);
                for (int i=0; i<8; i++)
                    if ( lights_enabled[i] ) glLightfv(GL_LIGHT0+i, GL_POSITION, lights[i].position);
            } else {
                glDisable(GL_LIGHTING);
            }
        }

///////////////////////////////////////////////////////////////////////////////////////////////
    void setGlobalAmbient(const float &a, const float &b,const float &c, const float &d=-1.0) {
            global_ambient_color[0] = a;
            global_ambient_color[1] = b;
            global_ambient_color[2] = c;
            if ( d != -1 ) global_ambient_color[3] = d;
            glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient_color); }

////////////////////////////////////////////////////////////////////////////////////////////////////////
    void setAmbient(const int &l, const float &a, const float &b, const float &c, const float &d=-1.0) {
            lights[l].ambient[0] = a;
            lights[l].ambient[1] = b;
            lights[l].ambient[2] = c;
            if ( d != -1 ) lights[l].ambient[3] = d;
            glLightfv(GL_LIGHT0+l, GL_AMBIENT, lights[l].ambient); }

////////////////////////////////////////////////////////////////////////////////////////////////////////
    void setDiffuse(const int &l, const float &a, const float &b, const float &c, const float &d=-1.0) {
            lights[l].diffuse[0] = a;
            lights[l].diffuse[1] = b;
            lights[l].diffuse[2] = c;
            if ( d != -1 ) lights[l].diffuse[3] = d;
            glLightfv(GL_LIGHT0+l, GL_DIFFUSE, lights[l].diffuse); }

/////////////////////////////////////////////////////////////////////////////////////////////////////////
    void setSpecular(const int &l, const float &a, const float &b, const float &c, const float &d=-1.0) {
            lights[l].specular[0] = a;
            lights[l].specular[1] = b;
            lights[l].specular[2] = c;
            if ( d != -1 ) lights[l].specular[3] = d;
            glLightfv(GL_LIGHT0+l, GL_SPECULAR, lights[l].specular); }

/////////////////////////////////////////////////////////////////////////////////////////////////////////
    vec3f getPosition(const int &l) { return vec3f(lights[l].position[0], lights[l].position[1], lights[l].position[2] ); }
    void setPosition(const int &l, const float &a, const float &b, const float &c, const float &d=-1.0) {
            lights[l].position[0] = a;
            lights[l].position[1] = b;
            lights[l].position[2] = c;
            if ( d != -1 ) lights[l].position[3] = d;
            glLightfv(GL_LIGHT0+l, GL_POSITION, lights[l].position); }


};

#endif
