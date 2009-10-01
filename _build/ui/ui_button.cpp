#include "sinui.h"

ui_button::ui_button() {

}
void ui_button::changeDims() {}
void ui_button::setInWH(int,int) {}

bool ui_button::eatKey(int,int) { return false; }
bool ui_button::eatKeyChar(int,int) { return false; }

bool ui_button::eatMousePos(int inx, int iny) {

    //first check to see if mouse is within the window
    if ( my_mouseover.isMouseover(inx,iny) ) {

        setHover(true);

        for ( cit=children.begin(); cit != children.end(); cit++ ) {
            if ( (*cit)->eatMousePos(inx,iny) ) {
                if ( hover_child != (*cit) && hover_child != NULL ) {
                    hover_child->setHover(false);
                }
                hover_child = (*cit);
                hover_child->setHover(true);
                return true;
            }
        }

        //no widget is moused over, so turn the rest off
        if ( hover_child != NULL ) {
            hover_child->setHover(false);
            hover_child = NULL;
        }

        //return true since the mouse was inside the window area
        return true;
    }

    return false;
}


/////////////////////////////////////////////////////////////////////////////////////////
//
bool ui_button::eatMouseClick(int button, int state, int inx, int iny ) {

    if ( !my_mouseover.isMouseover(inx, iny) ) return false;

    switch ( state ) {
        case GLFW_PRESS:
        switch ( button ) {
            case GLFW_MOUSE_BUTTON_LEFT:

                for (cit=children.begin(); cit != children.end(); cit++) {
                    if ( (*cit)->eatMouseClick(button,state,inx,iny) ) {
                        return true;
                    }
                }

                setPressed(true);

                if ( can_drag_parent ) {
                    setParentDragOffset(inx, iny);
                }

                return true;
            break;
        }
        break;

        case GLFW_RELEASE:
        switch ( button ) {
            case GLFW_MOUSE_BUTTON_LEFT:
                for (cit=children.begin(); cit != children.end(); cit++) {
                    if ( (*cit)->eatMouseClick(button,state,inx,iny) ) {
                        if ( (*cit)->is_active ) {
                            //if ( active_child != NULL && active_child != (*cit) ) active_child->setActive(false);
                            //active_child = (*cit);
                        }
                        setPressed(false);
                        return true;
                    }
                }

                if ( is_dragging ) is_dragging = false;
                if ( can_drag_parent ) stopParentDrag();
                if ( is_pressed ) { setActive(true); if ( payload != NULL ) { payload(); } }
                setPressed(false);

                return true;
            break;
        }
        break;
    }

    return false;
}


void ui_button::render() {}

void ui_button::redraw() {

    int cur_index = vec_texture_index;

    if ( is_pressed && my_style.has_pressed ) {
        my_style.pressed.addToVec(vec_texture,cur_index);
    } else if ( is_hovered && my_style.has_hover ) {
        my_style.hover.addToVec(vec_texture,cur_index);
    } else {
        my_style.normal.addToVec(vec_texture,cur_index);
    }

    for (int i=0; i < 4; i++) {
        vec_color->at(vec_color_index+3+4*i) = fade_percent;
    }

}

void ui_button::cook() {

    vec_texture_index = vec_texture->size();
    vec_vertex_index = vec_vertex->size();
    vec_color_index = vec_color->size();

    my_style.normal.addToVec(vec_texture);
    //
    vec_vertex->push_back( x );
    vec_vertex->push_back( y - h );
    vec_vertex->push_back( x );
    vec_vertex->push_back( y );
    vec_vertex->push_back( x + w );
    vec_vertex->push_back( y );
    vec_vertex->push_back( x + w );
    vec_vertex->push_back( y - h );

    for (int i=0; i < 4; i++) {
        vec_color->push_back(1.0);vec_color->push_back(1.0);vec_color->push_back(1.0);vec_color->push_back(1.0);
    }
}


/////////////////////////////////////////////////////////////////////////////////////////
//
void ui_button::doFade(int fade_dir, int fade_length) {

    static std::list<ui_base*>::iterator cit2;


    if ( (fade_dir == fade_direction ) ||
         (fade_dir == UI_FADE_IN && fade_percent == 1.0) ||
         (fade_dir == UI_FADE_OUT && fade_percent == 0.0) ) return;

    if ( fade_ticks == 0 ) { //no fade in progress
        fade_ticks = fade_length;
    } else { //fade in progress, continue where it left off
        fade_ticks = (1.0 - ((float)fade_ticks / (float)fade_ticks_max)) * (float)fade_length;
    }

    fade_direction = fade_dir;
    fade_ticks_max = fade_length;

    //fade all the  widgets
    for (cit2=children.begin(); cit2 != children.end(); cit2++) {
      (*cit2)->doFade(fade_dir, fade_length);
    }

}


/////////////////////////////////////////////////////////////////////////////////////////
//
void ui_button::update() {

    if ( fade_ticks ) {
        fade_ticks--;
        switch (fade_direction) {
            case UI_FADE_IN: fade_percent = 1.0 - (float)fade_ticks/(float)fade_ticks_max; break;
            case UI_FADE_OUT: fade_percent = (float)fade_ticks/(float)fade_ticks_max; break;
            default: break;
        }
        my_font.changeAlpha(fade_percent);
        redraw();
    }

    //update the widgets
    for (cit=children.begin(); cit != children.end(); cit++) {
      (*cit)->update();
    }
}
