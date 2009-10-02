
#include "sinui.h"

ui_base::ui_base() {
    x=y=w=h=0;
    drag_offset_x = 0;
    drag_offset_y = 0;
    fade_percent = 1.0;
    fade_direction = UI_FADE_NONE;
    fade_ticks = fade_ticks_max = 0;
    is_hovered = false;
    is_active = false;
    is_pressed = false;
    is_visible = true;
    is_dragging = false;
    can_drag_parent = false;
    parent = NULL;
    hover_child = NULL;
    active_child = NULL;
    my_controller = NULL;
    payload = NULL;
    custom_key_callback = NULL;
}

void ui_base::setXY(int inx, int iny) {
    x=inx+(parent == NULL ?0:parent->x);
    y=(parent == NULL ?iny:-iny+parent->y);
    my_mouseover.set(x,y-h,x+w,y);
    changeDims();
    }

void ui_base::setWH(int a, int b) {w=a; h=b; my_mouseover.set(x,y-h,x+w,y); changeDims(); }
void ui_base::setParent(ui_base *in) { parent = in; }
void ui_base::setPayload( void (*function)() ) { payload = function; }
void ui_base::setCustomKeyCallback( bool (*function)(int,int) ) { custom_key_callback = function; }

void ui_base::enableFontController() { my_controller = new freetype_font_controller(); }

void ui_base::setParentDragOffset(int inx, int iny) {
    if ( parent != NULL )
        { parent->setParentDragOffset(inx,iny); return; }

    drag_offset_x = inx; drag_offset_y = iny;
    is_dragging = true;
}

void ui_base::stopParentDrag() {
    if ( parent != NULL )
        { parent->stopParentDrag(); }

    is_dragging = false;
}

void ui_base::parentDrag(int inx, int iny) {

    int dragx = inx-drag_offset_x;
    int dragy = iny-drag_offset_y;
    std::vector<GLfloat>::iterator itt;

    for ( unsigned int i=0; i < vec_vertex->size(); i+=2 ) {
        vec_vertex->at(i) += dragx;
        vec_vertex->at(i+1) += dragy;
    }

    my_mouseover.moveBy(dragx,dragy);
    for (cit=children.begin(); cit != children.end(); cit++) {
        (*cit)->childDrag(dragx,dragy);
    }

    if ( my_controller != NULL ) my_controller->translate(dragx,dragy);

    drag_offset_x = inx; drag_offset_y = iny;
}

void ui_base::childDrag(int inx, int iny) {

    my_mouseover.moveBy(inx,iny);
    for (cit=children.begin(); cit != children.end(); cit++) {
        (*cit)->childDrag(inx,iny);
    }

}

void ui_base::setPointers(std::vector<GLfloat> *a, std::vector<GLfloat> *b, std::vector<GLfloat> *c ) {
    vec_vertex = a; vec_texture = b; vec_color = c; }

void ui_base::addChild( ui_base* in ) {
    children.push_back(in);
    in->setParent(this);
    has_children = true;
    in->setPointers(vec_vertex, vec_texture, vec_color);
    if ( my_controller != NULL ) {
        in->my_font.setController(my_controller);
    }
}


//virtuals
void ui_base::render() { printf("* STUB: ui_base::render\n"); }
void ui_base::update() { printf("* STUB: ui_base::update\n"); }
void ui_base::cook() { printf("* STUB: ui_base::cook\n"); }
void ui_base::changeDims() { printf("* STUB: ui_base::changeDims\n"); }
void ui_base::redraw() { printf("* STUB: ui_base::redraw\n"); }
void ui_base::setInWH(int,int) { printf("* STUB: ui_base::setInWH\n"); }
bool ui_base::eatMousePos(int,int) { printf("* STUB: ui_base::eatMousePos\n"); return false; }
bool ui_base::eatMouseClick(int,int,int,int) { printf("* STUB: ui_base::eatMouseClick\n"); return false; }
bool ui_base::eatKey(int,int) { printf("* STUB: ui_base::eatKey\n"); return false; }
bool ui_base::eatKeyChar(int,int) { printf("* STUB: ui_base::eatKeyChar\n"); return false; }
void ui_base::doFade(int,int) { printf("* STUB: ui_base::doFade\n"); }





void ui_base::setHover(bool in) {
    if ( is_hovered == in ) { return; }
    if ( in == true ) {
        is_hovered = true;
    } else {
        is_hovered = false;
        is_pressed = false;
        if ( hover_child != NULL ) {
            hover_child->setHover(false);
            hover_child = NULL;
        }
    }
    redraw();
}


void ui_base::setActive(bool in) {
    //printf("((%s:%d,%d))\n",my_name.c_str(),is_active,in);
    if ( is_active == in ) { return; }
    if ( in == true ) {
        if ( parent != NULL ) {
            parent->setActive(true);
            if ( parent->active_child != NULL && parent->active_child != this )
                parent->active_child->setActive(false);
            parent->active_child = this; }
        is_active = true;
    } else {
        //printf("setActive(FALSE) ", my_name.c_str());
        is_active = false;
        if ( active_child != NULL ) {
            //printf("setActiveChild(FALSE) ");
            active_child->setHover(false);
            active_child->setActive(false);
            active_child = NULL;
        }
    }
    redraw();
}


void ui_base::setPressed(bool in) {
    if ( is_pressed == in ) { return; }
    if ( in == true ) {
        is_pressed = true;
    } else {
        is_pressed = false;
    }
    redraw();
}
