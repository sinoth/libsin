#ifndef FONT_H
#define FONT_H

#include <ft2build.h>
#include FT_FREETYPE_H
#include <GL/glfw.h>

#include <list>
#include <map>
#include <vector>


class glyph_info {
public:

    char  x_offset;
    char  y_offset;
    char  character;
    float x_left;
    float x_right;
    float y_top;
    float y_bottom;
    short int advance;
    short int height;
    short int width;
    short int max_height;

    glyph_info( char, char, float, float, float, float, short int, short int, short int, char );

};


typedef struct glyph_matrixs {
    bool *bools;
    int w;
    int h;
    int xoffset;
    int yoffset;
    int advance;
} glyph_matrix;


class freetype_font {
public:

    int init(const char*,int,bool);
    int createText( const char*, std::vector<GLfloat>*, float, int, int, int );
    glyph_matrix* returnCharMatrix( char );
    int checkLength( char *, float );
    int returnOffset( char*, int, float );
    int clipText( char *, char **, char **, float, int);
    int clipTextMulti( char *, std::list<char*>*, float, int);
    static void repositionText( std::vector<GLfloat>*, int, int );
    GLuint getTextureID() { return atlas_texture; }


private:

    FT_Library    library;
    FT_Face       face;
    FT_GlyphSlot  slot;
    FT_Bitmap*    bitmap;
    GLuint        atlas_texture;
    GLubyte*      atlas_data;
    short int     max_height;
    bool          origin_topleft;
    glyph_matrix  *my_matrix;

    std::vector<glyph_info> glyphs;
    //need to figure out lists //?? wtf

    inline int next_p2 (int a ) { int rval=1; while(rval<a) rval<<=1; return rval; }

};


typedef struct font_list_pointerss {
    std::list<std::vector<GLfloat> *> vec_vertex_list;
    std::vector<GLfloat> *vec_texture_list;
    std::vector<GLfloat> *vec_color_list;
} font_list_pointers;

class freetype_font_controller {
public:

    void render();


private:

    //std::map<GLuint, font_pointers>


};

#endif //\/ FONT_H
