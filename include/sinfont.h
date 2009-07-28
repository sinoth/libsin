#ifndef FONT_H
#define FONT_H

#include <ft2build.h>
#include FT_FREETYPE_H
#include <GL/glfw.h>

#include <list>
#include <map>
#include <vector>


typedef struct languagess {

    void enableAll() { basic_latin = true;
                       latin_supplement = true;
                       latin_extended_a = true;
                       latin_extended_b = true;
                       ipa_extensions = true;
                       spacing_modifier_letters = true;
                       combining_diacritical_marks = true;
                       greek_coptic = true;
                       cyrillic = true;
                       cyrillic_supplement = true; }

    void enableLatin() { latin_supplement = true;
                         latin_extended_a = true;
                         latin_extended_b = true;
                         ipa_extensions = true;
                         spacing_modifier_letters = true;
                         combining_diacritical_marks = true; }

    void enableGreek() { greek_coptic = true; }

    void enableCyrillic() { cyrillic = true;
                            cyrillic_supplement = true; }

    void enableExtendedAscii() { latin_supplement = true; }


    languagess() : basic_latin(true),
                   latin_supplement(false),
                   latin_extended_a(false),
                   latin_extended_b(false),
                   ipa_extensions(false),
                   spacing_modifier_letters(false),
                   combining_diacritical_marks(false),
                   greek_coptic(false),
                   cyrillic(false),
                   cyrillic_supplement(false) {}

    void populate_list();

    bool basic_latin;
    bool latin_supplement;
    bool latin_extended_a;
    bool latin_extended_b;
    bool ipa_extensions;
    bool spacing_modifier_letters;
    bool combining_diacritical_marks;
    bool greek_coptic;
    bool cyrillic;
    bool cyrillic_supplement;

    std::list<short int> valid_chars;
    std::list<short int>::iterator it;

} languages;

//////////////////////////////////////////////////////////////////////////////////
//

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
    glyph_info() {};

};

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

    languages my_languages;

private:

    GLuint        atlas_texture;
    GLubyte*      atlas_data;
    bool          origin_topleft;
    GLfloat       r,g,b,a;
    int           max_height;

    std::map<short int, glyph_info> glyphs;
    std::map<short int, glyph_info>::iterator mit;

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
