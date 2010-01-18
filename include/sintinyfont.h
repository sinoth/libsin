#ifndef SINTINYFONT_H
#define SINTINYFONT_H

#include <GL/glfw.h>
#include <vector>
#include <sinprim.h>

typedef struct text_s {
    std::vector<GLfloat> vec_vertex;
    std::vector<GLfloat> vec_texture;
    std::vector<GLfloat> vec_color;
} tinytext;

typedef struct color_s {
    GLfloat r, g, b, a;
    color_s(GLfloat inr, GLfloat ing, GLfloat inb, GLfloat ina=1.0)
        { r=inr; g=ing; b=inb; a=ina; }
    color_s(int inr, int ing, int inb, int ina=255)
        { r=(GLfloat)inr/255.0; g=(GLfloat)ing/255.0; b=(GLfloat)inb/255.0; a=(GLfloat)ina/255.0; }
} tinycolor;


class tinyfont {
public:
    tinyfont() {}
    static bool init();
    static void beginRender();
    static void endRender();
    static void createText(tinytext &intext, const char *inphrase, const tinycolor &incolor, const vec3f &inpos);
    static void render(tinytext &in);

private:
    static GLuint texture;

};





#endif

