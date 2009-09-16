
#include "sinlight.h"

sinlight_s::sinlight_s()
{
    enabled = false;

    ambient[0] = 0.0; ambient[1] = 0.0; ambient[2] = 0.0; ambient[3] = 1.0;
    diffuse[0] = 0.0; diffuse[1] = 0.0; diffuse[2] = 0.0; diffuse[3] = 1.0;
    specular[0] = 0.0; specular[1] = 0.0; specular[2] = 0.0; specular[3] = 1.0;
    position[0] = 0.0; position[1] = 0.0; position[2] = 0.0; position[3] = 0.0;
}

void sinlight_s::setAmbient(float a,float b,float c,float d) {
    ambient[0] = a; ambient[1] = b; ambient[2] = c; if ( d != -1 ) ambient[3] = d; }

void sinlight_s::setDiffuse(float a,float b,float c,float d) {
    diffuse[0] = a; diffuse[1] = b; diffuse[2] = c; if ( d != -1 ) diffuse[3] = d; }

void sinlight_s::setSpecular(float a,float b,float c,float d) {
    specular[0] = a; specular[1] = b; specular[2] = c; if ( d != -1 ) specular[3] = d; }

void sinlight_s::setPosition(float a,float b,float c,float d) {
    position[0] = a; position[1] = b; position[2] = c; if ( d != -1 ) position[3] = d; }

void sinlight_s::enable() { enabled = true; }
void sinlight_s::disable() { enabled = false; }

////////////////////////////////////////
//
////////////////////////////////////////

sinlight_manager::sinlight_manager() {
    do_ambient = true;
    global_ambient_color[0] = 0.2;
    global_ambient_color[1] = 0.2;
    global_ambient_color[2] = 0.2;
    global_ambient_color[3] = 1.0;
}

void sinlight_manager::init() {

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);


}

void sinlight_manager::doLighting();

void sinlight_manager::setGlobalAmbient(float,float,float,float=-1.0);
void sinlight_manager::disableGlobalAmbient();
void sinlight_manager::enableGlobalAmbient();

void sinlight_manager::enableLight(int) {
}
void sinlight_manager::disableLight(int);
