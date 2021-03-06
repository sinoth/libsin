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

#define FONT_ALIGN_BOTTOM   1
#define FONT_ALIGN_CENTER   2
#define FONT_ALIGN_TOP      3
#define FONT_ALIGN_LEFT     4
#define FONT_ALIGN_RIGHT    5

#define FONT_HINT_STATIC    1
#define FONT_HINT_DYNAMIC   2

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
class font_object;
typedef struct font_list_pointerss {

    font_list_pointerss();
    void setStart();
    void clearAll();
    void setPointers( std::vector<GLfloat> *, std::vector<GLfloat> *, std::vector<GLfloat> *);
    void newPointers();

    std::vector<GLfloat> *vec_vertex;
    std::vector<GLfloat> *vec_texture;
    std::vector<GLfloat> *vec_color;
    int start_vertex;
    int start_color;
    int start_texture;
    bool active;

    font_list_pointerss *parent;
    font_object *last_font_object;

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
    void setColor( float* );
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
    int           newline_advance;

    std::map<short int, glyph_info> glyphs;
    std::map<short int, glyph_info>::iterator mit;

    font_list_pointers *my_list;

};


//////////////////////////////////////////////////////////////////////////////////
//
class font_object {

  public:
    void setFont( freetype_font* );
    void setFont( freetype_font*, int );
    void setText( const char* );
    //void addText( const char* );
    void setColor( float* );
    void setColor( float,float,float,float );
    void changeColor( float* );
    void changeAlpha( float );
    void setXY( int, int );
    //void scaleXY( float, float );
    void moveBy( int, int );
    void setMaxWH(int, int );
    void setActive( bool );
    void setHorizAlign( char );
    void setVertAlign( char );
    void setStretch( bool );
    void setController(class freetype_font_controller*);
    void selfRegister(int);
    void selfRegister(int,int);
    void setGroup(int);
    int  getGroup();
    int  getLength(const char*, float);

    bool isActive();
    bool isSet();
    void addChar(int);
    void backspace();
    void cook();

    font_object& operator= (const font_object);
    font_object();

  //why is this protected?
  //protected:
    void setHint(int);

  private:
    bool active;
    bool can_stretch;
    int group;
    int hint;
    int x, y;
    int max_width, max_height;
    int list_size;
    char horiz_align, vert_align;
    char *my_text;
    float color[4];
    freetype_font *my_font;
    font_list_pointers my_pointers;
    std::vector<GLfloat>::iterator it;
    freetype_font_controller *parent_controller;

    friend class freetype_font_controller;
};



class freetype_font_controller {
public:

    //for when we lose context
    void storeTexID();
    void restoreTexID();

    void render();
    void translate(int,int);
    //void scaleXY(float,float);
    void registerObject(font_object*);
    void registerObject(font_object*,int);
    void registerObject(font_object*,int,int);
    freetype_font_controller();

private:

    std::map<GLuint, std::list<font_list_pointers> > render_map;
    std::map<GLuint, std::list<font_list_pointers> > render_map_backup;
    std::map<GLuint, font_object* > render_map_lookup;
    std::map<GLuint, std::list<font_list_pointers> >::iterator it;
    std::list<font_list_pointers>::iterator lit;

};

#endif //\/ FONT_H
