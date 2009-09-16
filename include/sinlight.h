
#include <GL/glfw.h>

typedef struct sinlight_s
{
    float ambient[4];
    float diffuse[4];
    float specular[4];
    float position[4];
    bool enabled;

    sinlight_s();



} sinlight;


////////////////////////////////////////
//
////////////////////////////////////////


class sinlight_manager
{
public:

    void init();
    void doLighting();

    void setGlobalAmbient(float,float,float,float=-1.0);
    void disableGlobalAmbient();
    void enableGlobalAmbient();

    void setAmbient(int,float,float,float,float=-1.0);
    void setDiffuse(int,float,float,float,float=-1.0);
    void setSpecular(int,float,float,float,float=-1.0);
    void setPosition(int,float,float,float,float=-1.0);

    void enableLight(int);
    void disableLight(int);

    sinlight lights[8];

    sinlight_manager();

private:

    bool do_ambient;
    float global_ambient_color[4];

};
