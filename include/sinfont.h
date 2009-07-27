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

    glyph_info( char, char, float, float, float, float, short int, short int, short int, char );

};

typedef struct glyph_matrixs {
//    bool *bools;
    int w;
    int h;
    int xoffset;
    int yoffset;
    int advance;
} glyph_matrix;

//////////////////////////////////////////////////////////////////////////////////
//

typedef struct font_list_pointerss {
    std::vector<GLfloat> *vec_vertex;
    std::vector<GLfloat> *vec_texture;
    std::vector<GLfloat> *vec_color;
    int start_vertex;
    int start_color;
    int start_texture;

    font_list_pointerss() : start_vertex(0),
                            start_color(0),
                            start_texture(0)
                            { vec_vertex = NULL;
                              vec_texture = NULL;
                              vec_color = NULL;   }

    void setStart() { start_vertex = vec_vertex->size();
                      start_texture = vec_texture->size();
                      start_color = vec_color->size(); }

    void clearAll() { vec_vertex->clear();
                      vec_texture->clear();
                      vec_color->clear(); }

    void setPointers( std::vector<GLfloat> *invec, std::vector<GLfloat> *intex, std::vector<GLfloat> *incol ) {
            vec_vertex = invec; vec_texture = intex; vec_color = incol; }

} font_list_pointers;

//////////////////////////////////////////////////////////////////////////////////
//

class freetype_font {
public:

    int init(const char*,int,bool);
    int createText( const char*, std::vector<GLfloat>*, float, int, int, int );
    //glyph_matrix* returnCharMatrix( char );
    int checkLength( const char *, float );
    int returnOffset( char*, int, float );
    int clipText( char *, char **, char **, float, int);
    int clipTextMulti( char *, std::list<char*>*, float, int);
    static void repositionText( std::vector<GLfloat>*, int, int );
    GLuint getTextureID() { return atlas_texture; }
    void setColor( float inr, float ing, float inb, float ina ) { r=inr; g=ing; b=inb; a=ina; }
    void setPointerList( font_list_pointers *in ) { my_list = in; }
    int getMaxHeight() { return max_height; }

    freetype_font() { my_list = NULL; max_height = 0;}

private:

    FT_Library    library;
    FT_Face       face;
    FT_GlyphSlot  slot;
    FT_Bitmap*    bitmap;
    GLuint        atlas_texture;
    GLubyte*      atlas_data;
    bool          origin_topleft;
    //glyph_matrix  *my_matrix;
    GLfloat       r,g,b,a;
    int           max_height;

    std::vector<glyph_info> glyphs;

    font_list_pointers *my_list;

    inline int next_p2 (int a ) { int rval=1; while(rval<a) rval<<=1; return rval; }

};


//////////////////////////////////////////////////////////////////////////////////
//

class freetype_font_controller {
public:

    void render() {

            glEnable(GL_TEXTURE_2D);
            glEnable(GL_BLEND);
            glEnableClientState(GL_VERTEX_ARRAY);
            glEnableClientState(GL_COLOR_ARRAY);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);

            for ( it=all_fonts.begin(); it != all_fonts.end(); it++ ) {

                glBindTexture( GL_TEXTURE_2D, (*it).first );

                //for every font in the list, we have to go through every set of pointers
                for (lit=(*it).second.begin(); lit != (*it).second.end(); lit++ ) {
                    glVertexPointer(3, GL_FLOAT, 0, &(*lit)->vec_vertex->at(0) );
                    glTexCoordPointer(2, GL_FLOAT, 0, &(*lit)->vec_texture->at(0) );
                    glColorPointer(4, GL_FLOAT, 0, &(*lit)->vec_color->at(0) );
                    glDrawArrays(GL_TRIANGLE_STRIP, 0, (*lit)->vec_vertex->size()/3 );
                    //glInterleavedArrays(GL_T2F_V3F,0,&(*lit)->vec_vertex->at(0) );
                }
            }

            glDisableClientState(GL_VERTEX_ARRAY);
            glDisableClientState(GL_COLOR_ARRAY);
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            glDisable(GL_BLEND);
            glDisable(GL_TEXTURE_2D);

        }

    font_list_pointers registerFont( freetype_font *in, bool is_static ) {
            if ( is_static ) {
                if ( all_fonts.find( in->getTextureID() ) == all_fonts.end() ) {
                    //not in the map
                    font_list_pointers *new_fontp = new font_list_pointers;
                    new_fontp->setPointers( new std::vector<GLfloat>, new std::vector<GLfloat>, new std::vector<GLfloat> );
                    all_fonts[in->getTextureID()].push_front(new_fontp);
                    printf("* FONT - Adding new font texture %d [static]\n", in->getTextureID() );
                    return *new_fontp;
                }
                //already exists
                all_fonts[in->getTextureID()].front()->setStart();
                printf("* FONT - Existing font texture %d [static]\n", in->getTextureID() );
                return *all_fonts[in->getTextureID()].front();

            } else {
                //not static, give it a new list
                font_list_pointers *new_fontp = new font_list_pointers;
                new_fontp->setPointers( new std::vector<GLfloat>, new std::vector<GLfloat>, new std::vector<GLfloat> );
                all_fonts[in->getTextureID()].push_back(new_fontp);
                printf("* FONT - Adding new font texture %d [dynamic]\n", in->getTextureID() );
                return *new_fontp;
            }
        }

private:

    std::map<GLuint, std::list<font_list_pointers*> > all_fonts;
    std::map<GLuint, std::list<font_list_pointers*> >::iterator it;
    std::list<font_list_pointers*>::iterator lit;

};

#endif //\/ FONT_H
