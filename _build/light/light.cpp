
#include "sinlight.h"

sinlight_s::sinlight_s()
{
    ambient[0] = 0.0; ambient[1] = 0.0; ambient[2] = 0.0; ambient[3] = 1.0;
    diffuse[0] = 0.0; diffuse[1] = 0.0; diffuse[2] = 0.0; diffuse[3] = 1.0;
    specular[0] = 0.0; specular[1] = 0.0; specular[2] = 0.0; specular[3] = 1.0;
    position[0] = 0.0; position[1] = 0.0; position[2] = 0.0; position[3] = 0.0;
}

////////////////////////////////////////
//
////////////////////////////////////////

sinLighting::sinLighting() {
    do_ambient = true;
    enabled = true;
    global_ambient_color[0] = 0.2;
    global_ambient_color[1] = 0.2;
    global_ambient_color[2] = 0.2;
    global_ambient_color[3] = 1.0;
    none[0] = 0.0; none[1] = 0.0; none[2] = 0.0; none[3] = 0.0;
    for (int i=0; i<8; i++) lights_enabled[i] = false;
}

void sinLighting::init() {
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    //glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, .001);
    //glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.02f);
    //glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.008f);
}

void sinLighting::doLighting() {
    if ( enabled ) {
        glEnable(GL_LIGHTING);
        for (int i=0; i<8; i++)
            if ( lights_enabled[i] ) glLightfv(GL_LIGHT0+i, GL_POSITION, lights[i].position);
    } else {
        glDisable(GL_LIGHTING);
    }
}

void sinLighting::setAmbient(int l, float a,float b,float c,float d) {
    lights[l].ambient[0] = a; lights[l].ambient[1] = b; lights[l].ambient[2] = c; if ( d != -1 ) lights[l].ambient[3] = d;
    glLightfv(GL_LIGHT0+l, GL_AMBIENT, lights[l].ambient); }

void sinLighting::setDiffuse(int l, float a,float b,float c,float d) {
    lights[l].diffuse[0] = a; lights[l].diffuse[1] = b; lights[l].diffuse[2] = c; if ( d != -1 ) lights[l].diffuse[3] = d;
    glLightfv(GL_LIGHT0+l, GL_DIFFUSE, lights[l].diffuse); }

void sinLighting::setSpecular(int l, float a,float b,float c,float d) {
    lights[l].specular[0] = a; lights[l].specular[1] = b; lights[l].specular[2] = c; if ( d != -1 ) lights[l].specular[3] = d;
    glLightfv(GL_LIGHT0+l, GL_SPECULAR, lights[l].specular); }

void sinLighting::setPosition(int l, float a,float b,float c,float d) {
    lights[l].position[0] = a; lights[l].position[1] = b; lights[l].position[2] = c; if ( d != -1 ) lights[l].position[3] = d;
    glLightfv(GL_LIGHT0+l, GL_POSITION, lights[l].position); }

void sinLighting::setGlobalAmbient(float a,float b,float c,float d) {
    global_ambient_color[0] = a; global_ambient_color[1] = b; global_ambient_color[2] = c; if ( d != -1 ) global_ambient_color[3] = d;
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient_color); }

void sinLighting::disableGlobalAmbient() { glLightModelfv(GL_LIGHT_MODEL_AMBIENT, none); }
void sinLighting::enableGlobalAmbient() { glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient_color); }

void sinLighting::enableLight(int l) { glEnable(GL_LIGHT0+l); }
void sinLighting::disableLight(int l) { glDisable(GL_LIGHT0+l); }

void sinLighting::enable() { enabled = true; }
void sinLighting::disable() { enabled = false; }
