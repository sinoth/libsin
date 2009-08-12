/////////////////////////////
// todo
//
// [ ] make a more general interface for adding text to the scene
//     one controller should be able to take care of the whole screen
//     allow user to hide and unhide specific text
//
//     basically you feed the font engine text, it gives you back a unique ID
//     you use this ID to do stuff with the text.. move it, change color, etc
//


#ifndef FONT_H
#define FONT_H

#include <ft2build.h>
#include FT_FREETYPE_H
#include <GL/glfw.h>

#include <list>
#include <map>
#include <vector>


#define FONT_ORIGIN_LOWERLEFT 1
#define FONT_ORIGIN_UPPERLEFT 2

typedef struct languagess {

    void enableAll();
    void enableLatin();
    void enableGreek();
    void enableCyrillic();
    void enableExtendedAscii();
    void populate_list();
    languagess();

    ////

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

    font_list_pointerss();
    void setStart();
    void clearAll();
    void setPointers( std::vector<GLfloat> *, std::vector<GLfloat> *, std::vector<GLfloat> *);

    std::vector<GLfloat> *vec_vertex;
    std::vector<GLfloat> *vec_texture;
    std::vector<GLfloat> *vec_color;
    int start_vertex;
    int start_color;
    int start_texture;

} font_list_pointers;

//////////////////////////////////////////////////////////////////////////////////
//

class freetype_font {
public:

    int init(const char*,int);
    int createText( const char*, std::vector<GLfloat>*, float, int, int, int );
    int checkLength( const char *, float );
    int returnOffset( char*, int, float );
    int clipText( char *, char **, char **, float, int);
    int clipTextMulti( char *, std::list<char*>*, float, int);
    GLuint getTextureID();
    void setColor( float, float, float, float );
    void setColor( int, int, int, int );
    void setPointerList( font_list_pointers * );
    int getMaxHeight();
    void setCorner(int);

    static void repositionText( std::vector<GLfloat>*, int, int );

    freetype_font();

    languages my_languages;

private:

    int next_p2(int);

    GLuint        atlas_texture;
    GLubyte*      atlas_data;
    bool          origin_topleft;
    GLfloat       r,g,b,a;
    int           max_height;

    std::map<short int, glyph_info> glyphs;
    std::map<short int, glyph_info>::iterator mit;

    font_list_pointers *my_list;

};


//////////////////////////////////////////////////////////////////////////////////
//

class freetype_font_controller {
public:

    void render();
    void moveTo(int, int);
    font_list_pointers registerFont( freetype_font *, bool );

private:

    std::map<GLuint, std::list<font_list_pointers*> > all_fonts;
    std::map<GLuint, std::list<font_list_pointers*> >::iterator it;
    std::list<font_list_pointers*>::iterator lit;

};

#endif //\/ FONT_H
