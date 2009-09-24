#include "sinfont.h"


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
//

font_object::font_object() {
    active = true;
    can_stretch = false;
    group = -1;
    hint = 0;
    x = 0; y = 0;
    max_width = 0; max_height = 0;
    list_size = 0;
    memset(color, 0, sizeof(float)*4 );
    my_font = NULL;
    my_text = NULL;
    horiz_align = FONT_ALIGN_LEFT;
    vert_align = FONT_ALIGN_TOP;
}

void font_object::setFont( freetype_font *in ) { my_font = in; parent_controller->registerObject(this); }
void font_object::selfRegister(int in) { parent_controller->registerObject(this, in); }
void font_object::selfRegister(int in, int group) { parent_controller->registerObject(this, in, group); }
void font_object::setColor( float *in ) { memcpy( color, in, sizeof(float)*4 ); }
void font_object::setColor( float inr, float ing, float inb, float ina ) { color[0]=inr; color[1]=ing; color[2]=inb; color[3]=ina; }
void font_object::setXY( int inx, int iny ) { x = inx; y = iny; }
void font_object::setMaxWH( int inw, int inh ) { max_width = inw; max_height = inh; }
void font_object::setActive( bool in ) { active = in; if ( hint == FONT_HINT_DYNAMIC ) my_pointers.parent->active = in; }
void font_object::setHorizAlign( char in ) { horiz_align = in; }
void font_object::setVertAlign( char in ) { vert_align = in; }
void font_object::setHint(int in) { hint = in; }
void font_object::setStretch(bool in) { can_stretch = in; }
void font_object::setController(freetype_font_controller_omega *in) { parent_controller = in; }
void font_object::setGroup(int in) { group = in; }
int  font_object::getGroup() { return group; }
int  font_object::getLength(const char* inc, float inf) { return my_font->checkLength(inc, inf); }
bool font_object::isActive() { return active; }

////////////////////////////////////////////////////////////////////////////
//
void font_object::setText( const char *in ) {
    if ( my_text != NULL && hint == FONT_HINT_STATIC ) {
        printf("* ERROR: cannot set static text more than once\n"); return; }

    my_text = (char*) malloc( strlen(in)*sizeof(char)+1 );
    strcpy(my_text,in);
}

////////////////////////////////////////////////////////////////////////////
//
void font_object::changeColor( float* in ) {
    it = my_pointers.vec_color->begin();
    it += my_pointers.start_color;

    for (int i=0; i < list_size; i++ ) {
        (*it++) = in[0];
        (*it++) = in[1];
        (*it++) = in[2];
        (*it++) = in[3];
    }
}

////////////////////////////////////////////////////////////////////////////
//
void font_object::changeAlpha(float in) {
    if ( my_pointers.vec_vertex != NULL ) {
        it = my_pointers.vec_color->begin();
        it += my_pointers.start_color +  3;

        for (int i=0; i < list_size; i++ ) {
            (*it) = in;
            it+=4;
        }
    }
}

void font_object::cook() {

    if ( hint == 0 ) {
        printf("* ERROR: font object must be registered before cooking\n"); return; }
    if ( my_text == NULL ) {
        printf("* ERROR: font text must be set before cooking\n"); return; }

    if ( hint == FONT_HINT_DYNAMIC ) {
        my_pointers.clearAll();
    }

    int text_width = my_font->checkLength(my_text, 1.0);
    int text_height = my_font->getMaxHeight();
    float text_scale = 1.0;
    int size_before = 0;

    //printf("s: %s, w: %d, h: %d, s: %f, max_width: %d, max_height: %d\n", my_text, text_width, text_height, text_scale, max_width, max_height);

    if ( text_height > max_height ) { text_scale = (float)max_height/(float)text_height; } else { text_scale = 1.0; }
    if ( text_width * text_scale > max_width ) { text_scale = (float)max_width / ((float)text_width* text_scale); }
    text_width = text_width * text_scale;
    text_height = text_height * text_scale;

    if ( can_stretch && text_scale == 1.0 ) {
        text_scale = max_height / text_height;
        if ( text_width * text_scale > max_width ) { text_scale *= max_width / ((float)text_width* text_scale); }
        text_width = text_width * text_scale;
        text_height = text_height * text_scale;
    }

    int xoffset, yoffset;
    switch ( horiz_align ) {
        case FONT_ALIGN_LEFT:    xoffset = 0; break;
        case FONT_ALIGN_CENTER:  xoffset = (float)max_width/2.0 - (float)text_width/2.0; break;
        case FONT_ALIGN_RIGHT:   xoffset = max_width - text_width; break;
        default:                 xoffset = 0; break;
    }
    switch ( vert_align ) {
        case FONT_ALIGN_TOP:     yoffset = 0; break;
        case FONT_ALIGN_CENTER:  yoffset = -(float)max_height/2.0 + (float)text_height/2.0 - 0; break; //extra -1 since most chars are not low
        case FONT_ALIGN_BOTTOM:  yoffset = -max_height + text_height; break;
        default:                 yoffset = 0; break;
    }

    //printf("s: %s, w: %d, h: %d, s: %f, xo: %d, yo: %d, max_width: %d\n", my_text, text_width, text_height, text_scale, xoffset, yoffset, max_width);

    switch (hint) {
        case FONT_HINT_STATIC:
            my_font->setPointerList( &my_pointers );
            size_before = my_pointers.vec_vertex->size();

            my_font->setColor( color );
            my_font->createText( my_text, NULL, text_scale, x+xoffset, y+yoffset, 0 );
            list_size = (my_pointers.vec_vertex->size() - size_before) / 3;
            break;

        case FONT_HINT_DYNAMIC:
            my_font->setPointerList( &my_pointers );
            //my_pointers.clearAll();

            my_font->setColor( color );
            my_font->createText( my_text, NULL, text_scale, x+xoffset, y+yoffset, 0 );
            list_size = my_pointers.vec_vertex->size() / 3;
            break;
    }

}

font_object& font_object::operator= (const font_object rhs) {
    if ( this != &rhs ) {
        active = rhs.active;
        can_stretch = rhs.can_stretch;
        hint = rhs.hint;
        x = rhs.x;
        y = rhs.y;
        max_width = rhs.max_width;
        max_height = rhs.max_height;
        horiz_align = rhs.horiz_align;
        vert_align = rhs.vert_align;
        memcpy(color,rhs.color,sizeof(float)*4);
        my_font = rhs.my_font;
        parent_controller = rhs.parent_controller;

        //my_text = NULL;
        //list_size = 0;
        //my_pointers;
    }

    return *this;
}




/*
void font_object::addText( const char *in ) {

    if ( hint == 0 ) {
        printf("* ERROR: font object must be registered before cooking\n"); return; }
    if ( hint != FONT_HINT_DYNAMIC ) {
        printf("* ERROR: addText must be used on a dynamic object\n"); return; }

    int text_width = my_font->checkLength(my_text, 1.0);
    int text_height = my_font->getMaxHeight();
    int text_scale = 1.0;

    if ( text_height > max_height ) { text_scale = (float)max_height/(float)text_height; } else { text_scale = 1.0; }
    if ( text_width * text_scale > max_width ) { text_scale = (float)max_width / ((float)text_width* text_scale); }
    text_width = text_width * text_scale;
    text_height = text_height * text_scale;

    if ( can_stretch && text_scale == 1.0 ) {
        text_scale = max_height / text_height;
        if ( text_width * text_scale > max_width ) { text_scale = max_width / ((float)text_width* text_scale); }
        text_width = text_width * text_scale;
        text_height = text_height * text_scale;
    }

    int xoffset, yoffset;
    switch ( horiz_align ) {
        case FONT_ALIGN_LEFT:    xoffset = 0; break;
        case FONT_ALIGN_CENTER:  xoffset = (float)max_width/2.0 - (float)text_width/2.0; break;
        case FONT_ALIGN_RIGHT:   xoffset = max_width - text_width; break;
        default:                 xoffset = 0; break;
    }
    switch ( vert_align ) {
        case FONT_ALIGN_TOP:     yoffset = 0; break;
        case FONT_ALIGN_CENTER:  yoffset = -(float)max_height/2.0 + (float)text_height/2.0 - 0; break; //extra -1 since most chars are not low
        case FONT_ALIGN_BOTTOM:  yoffset = -max_height + text_height; break;
        default:                 yoffset = 0; break;
    }

    my_font->setPointerList( &my_pointers );
    my_font->setColor( color );
    my_font->createText( my_text, NULL, text_scale, x+xoffset, y+yoffset, 0 );
    list_size = my_pointers.vec_vertex->size() / 3;

}
*/
