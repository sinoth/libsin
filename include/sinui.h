#ifndef _SINUI_H_2
#define _SINUI_H_2


#include <GL/glfw.h>
#include <list>
#include <vector>
#include <string>
#include <map>
#include <sinfont.h>
#include <sintimer.h>
//#include <dsfmt.h>


//widget types
#define UI_WIDGET_BASE      0
#define UI_WIDGET_BUTTON    1
#define UI_WIDGET_CHECKBOX  2
#define UI_WIDGET_LABEL     3
#define UI_WIDGET_LISTBOX   4
#define UI_WIDGET_BUTTON_NP 5
#define UI_WIDGET_TEXTINPUT 6

//alignment
//#define UI_ALIGN_TOP     1
//#define UI_ALIGN_MIDDLE  2
//#define UI_ALIGN_BOTTOM  3
//#define UI_ALIGN_LEFT    4
//#define UI_ALIGN_RIGHT   5

//fading
#define UI_FADE_NONE    0
#define UI_FADE_IN      1
#define UI_FADE_OUT     2

//ninepatch helper
#define UI_9P_TL    0
#define UI_9P_TM    1
#define UI_9P_TR    2
#define UI_9P_ML    3
#define UI_9P_MM    4
#define UI_9P_MR    5
#define UI_9P_BL    6
#define UI_9P_BM    7
#define UI_9P_BR    8


typedef struct tex_info_s {
    float x_l, x_r;
    float y_t, y_b;
    short w, h;
    //
    void setTexXY(float,float,float,float);
    void setTex(float,float,float,float,float,float);
    void setWH(short,short);
    void addToVec( std::vector<GLfloat> * );
    void addToVec( std::vector<GLfloat> *, int& );
    tex_info_s();
} tex_info;
/////////////////////////////////////////////
typedef struct np_object_s {
    tex_info my_np[9];
    //
    void setNP(int,float,float,float,float,float,float);
} np_object;
/////////////////////////////////////////////
typedef struct np_style_s {
    int min_x, min_y;
    int font_x, font_y;
    int font_w_off, font_h_off;
    np_object normal;
    np_object active;
    np_object hover;
    np_object pressed;
    bool has_active;
    bool has_hover;
    bool has_pressed;
    //
    void cook();
    np_style_s();
} np_style;
/////////////////////////////////////////////
typedef struct tex_style_s {
    tex_info_s normal;
    tex_info_s active;
    tex_info_s hover;
    tex_info_s pressed;
    bool has_active;
    bool has_hover;
    bool has_pressed;
    //
    void cook();
    tex_style_s();
} tex_style;
////////////////////////////////////////////////////////////////////////////
typedef struct mouseover_s {
    bool isMouseover(int, int);
    void set(int, int, int, int);
    void moveBy(int, int);
    mouseover_s();
    //
    int X1, Y1, X2, Y2; // x1,y1 is lower left - x2,y2 is upper right
} mouseover;

////////////////////////////////////////////////////////////////////////////
//
class ui_base {
  public:
    int x, y;
    int w, h;
    int drag_offset_x;
    int drag_offset_y;
    bool is_hovered;
    bool is_active;
    bool is_pressed;
    bool is_dragging;
    bool is_visible;
    bool has_children;
    bool can_drag_parent;
    int vec_texture_index;
    int vec_vertex_index;
    int vec_color_index;
    std::vector<GLfloat> *vec_vertex;
    std::vector<GLfloat> *vec_texture;
    std::vector<GLfloat> *vec_color;
    mouseover my_mouseover;
    ui_base *parent;

    std::string my_name;

    float fade_percent;
    int fade_direction;
    int fade_ticks;
    int fade_ticks_max;

    void addChild(ui_base*);
    std::list<ui_base*> children;
    std::list<ui_base*>::iterator cit;
    ui_base *hover_child;
    ui_base *active_child;
    //
    void moveBy(int,int);
    void setParentDragOffset(int,int);
    void stopParentDrag();
    void parentDrag(int,int);
    void childDrag(int,int);

    void (*payload)();
    bool (*custom_key_callback)(int,int);
    void setPayload(void(*)());
    void setCustomKeyCallback(bool(*)(int,int));

    font_object my_font;
    freetype_font_controller *my_controller;
    void enableFontController();
    void setXY(int,int);
    void setWH(int,int);
    void setParent(ui_base*);
    void setHover(bool);
    void setActive(bool);
    void setPressed(bool);
    void setPointers(std::vector<GLfloat> *,std::vector<GLfloat> *,std::vector<GLfloat> *);
    ui_base();
    //
    virtual void render();
    virtual void update();
    virtual void cook();
    virtual void redraw();
    virtual void setInWH(int,int);
    virtual void changeDims();
    virtual bool eatMousePos(int,int);
    virtual bool eatMouseClick(int,int,int,int);
    virtual bool eatKey(int,int);
    virtual bool eatKeyChar(int,int);
    virtual void doFade(int,int);
};

////////////////////////////////////////////////////////////////////////////
//
class ui_button : public ui_base {
  public:
    tex_style my_style;
    //
    ui_button();
    // virtuals
    void render();
    void update();
    void cook();
    void redraw();
    void setInWH(int,int);
    void changeDims();
    bool eatMousePos(int,int);
    bool eatMouseClick(int,int,int,int);
    bool eatKey(int,int);
    bool eatKeyChar(int,int);
    void doFade(int,int);
};

class ui_button_np : public ui_base {
  public:
    np_style my_style;
    //
    void setStyle(np_style &);
    ui_button_np();
    // virtuals
    void render();
    void update();
    void cook();
    void redraw();
    void setInWH(int,int);
    void changeDims();
    bool eatMousePos(int,int);
    bool eatMouseClick(int,int,int,int);
    bool eatKey(int,int);
    bool eatKeyChar(int,int);
    void doFade(int,int);
};

////////////////////////////////////////////////////////////////////////////
//
class ui_textinput : public ui_base {
  public:
    np_style my_style;
    std::string my_text;
    unsigned int max_text_length;
    bool doing_backspace;
    bool doing_backspace_wait;
    int backspace_delay;
    int backspace_repeat_delay;
    sinTimer backspace_timer;
    sinTimer backspace_repeat_timer;

    sinTimer keyrepeat_timer;
    sinTimer keyrepeat_timer_repeat;
    bool doing_keyrepeat_wait;
    bool doing_keyrepeat;
    int keyrepeat_delay;
    int keyrepeat_repeat;
    int last_char;

    //cursor stuff
    bool cursor_is_blinking;
    int  cursor_x_size;
    int  cursor_y_size_offset;
    int  cursor_x_offset;

    //
    void setStyle(np_style &);
    void setText(const char*);
    void setMaxTextLength(unsigned int);
    void addChar(char);
    void deleteChar();

    void setCursorSize(int);
    char *getText();
    ui_textinput();

    // virtuals
    void render();
    void update();
    void cook();
    void redraw();
    void setInWH(int,int);
    void changeDims();
    bool eatMousePos(int,int);
    bool eatMouseClick(int,int,int,int);
    bool eatKey(int,int);
    bool eatKeyChar(int,int);
    void doFade(int,int);
};

////////////////////////////////////////////////////////////////////////////
//
class ui_label : public ui_base {
  public:
    ui_label();
    // virtuals
    void render();
    void update();
    void cook();
    void redraw();
    void setInWH(int,int);
    void changeDims();
    bool eatMousePos(int,int);
    bool eatMouseClick(int,int,int,int);
    bool eatKey(int,int);
    bool eatKeyChar(int,int);
    void doFade(int,int);
};

////////////////////////////////////////////////////////////////////////////
//
class ui_checkbox : public ui_base {
  public:
    bool is_checked;
    bool show_check_always;
    np_style my_style;
    tex_info checkmark;

    void setStyle(np_style &);
    ui_checkbox();
    // virtuals
    void render();
    void update();
    void cook();
    void redraw();
    void setInWH(int,int);
    void changeDims();
    bool eatMousePos(int,int);
    bool eatMouseClick(int,int,int,int);
    bool eatKey(int,int);
    bool eatKeyChar(int,int);
    void doFade(int,int);
};

////////////////////////////////////////////////////////////////////////////
//
class ui_window : public ui_base {
  public:
    np_style window_style;
    ui_button closebutton;
    ui_button_np titlebar;
    bool show_closebutton;
    bool show_titlebar;
    bool can_resize;
    GLuint theme_texture;

    std::vector<GLfloat> real_vec_vertex;
    std::vector<GLfloat> real_vec_texture;
    std::vector<GLfloat> real_vec_color;
    //
    void enableCloseButton();
    void enableTitlebar();
    void setThemeTextureID(GLuint);
    ui_window();
    // virtuals
    void render();
    void update();
    void cook();
    void redraw();
    void setInWH(int,int);
    void changeDims();
    bool eatMousePos(int,int);
    bool eatMouseClick(int,int,int,int);
    bool eatKey(int,int);
    bool eatKeyChar(int,int);
    void doFade(int,int);
};



////////////////////////////////////////////////////////////////////////////
//
class SinUI {

public:

    void cook();
    void render();
    void update();

    bool insertMousePos(int, int);
    bool insertMouseClick(int, int, int, int);
    bool insertKey(int, int);
    bool insertKeyChar(int, int);

    ui_window* addWindow(const char*);
    ui_window* getWindow(const char*);
    ui_base* addWidget(const char*, int);
    ui_base* getWidget(const char*);

    ui_window* getActiveWindow();
    void bringToFront(ui_window* in);

    SinUI();

private:

    ui_window *draggingWindow;
    ui_window *activeWindow;
    ui_window *hoverWindow;
    ui_window *lastClickedWindow;

    std::list<ui_window*> window_list;
    std::list<ui_window*>::iterator it;
    std::list<ui_window*>::reverse_iterator rit;

    std::map<std::string, ui_window*> window_map;
    std::map<std::string, ui_window*>::iterator window_map_it;
    std::map<std::string, ui_base*> widget_map;
    std::map<std::string, ui_base*>::iterator widget_map_it;

};


#endif
