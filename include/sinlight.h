
#include <GL/glfw.h>

typedef struct sinlight_s
{
    float ambient[4];
    float diffuse[4];
    float specular[4];
    float position[4];

    sinlight_s();

} sinlight;


////////////////////////////////////////
//
////////////////////////////////////////


class sinLighting
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
    void enable();
    void disable();

    sinlight lights[8];
    bool lights_enabled[8];

    sinLighting();

private:

    bool enabled;
    bool do_ambient;
    float global_ambient_color[4];
    float none[4];

};
