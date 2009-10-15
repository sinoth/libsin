
#include "sinui.h"

ui_label::ui_label() {
}
void ui_label::changeDims() {
    my_font.setXY(x,y);
    my_font.setMaxWH(w,h);
}
void ui_label::setInWH(int,int) {}

bool ui_label::eatKey(int,int) { return false; }
bool ui_label::eatKeyChar(int,int) { return false; }
bool ui_label::eatMousePos(int, int) { return false; }
void ui_label::render() {}

bool ui_label::eatMouseClick(int button, int state, int inx, int iny) {

    if ( !my_mouseover.isMouseover(inx, iny) ) return false;

    switch ( state ) {
        case GLFW_PRESS:
        switch ( button ) {
            case GLFW_MOUSE_BUTTON_LEFT:

                //for (cit=children.begin(); cit != children.end(); cit++) {
                //    if ( (*cit)->eatMouseClick(button,state,inx,iny) ) {
                //        return true;
                //    }
                //}

                setPressed(true);

                //if ( can_drag_parent ) {
                //    setParentDragOffset(inx, iny);
                //}

                return true;
            break;
        }
        break;

        case GLFW_RELEASE:
        switch ( button ) {
            case GLFW_MOUSE_BUTTON_LEFT:

                //if ( is_dragging ) is_dragging = false;
                //if ( can_drag_parent ) stopParentDrag();
                if ( is_pressed ) {
                    //setActive(true);
                    if ( payload != NULL ) { payload(); }
                    }
                setPressed(false);

                return true;
            break;
        }
        break;
    }

    return false;
}


void ui_label::redraw() {

    if ( is_pressed ) {
    } else if ( is_hovered ) {
    } else {
    }

}

void ui_label::cook() {
}


/////////////////////////////////////////////////////////////////////////////////////////
//
void ui_label::doFade(int fade_dir, int fade_length) {

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

}


/////////////////////////////////////////////////////////////////////////////////////////
//
void ui_label::update() {

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

}
