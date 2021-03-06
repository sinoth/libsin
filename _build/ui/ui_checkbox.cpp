#include "sinui.h"

ui_checkbox::ui_checkbox() {
    is_checked = false;
    show_check_always = false;
}

bool ui_checkbox::isChecked() { return is_checked; }
void ui_checkbox::flipChecked() { is_checked = !is_checked; redraw(); }
void ui_checkbox::changeDims() {}
void ui_checkbox::setInWH(int,int) {}
void ui_checkbox::setStyle( np_style &in ) { my_style = in; }

bool ui_checkbox::eatKey(int,int) { return false; }
bool ui_checkbox::eatKeyChar(int,int) { return false; }

bool ui_checkbox::eatMousePos(int inx, int iny) {

    //first check to see if mouse is within the window
    if ( my_mouseover.isMouseover(inx,iny) ) {
        setHover(true);
        return true;
    }

    return false;
}


/////////////////////////////////////////////////////////////////////////////////////////
//
bool ui_checkbox::eatMouseClick(int button, int state, int inx, int iny ) {

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

                if ( is_dragging ) is_dragging = false;
                if ( can_drag_parent ) stopParentDrag();
                if ( is_pressed ) {
                    setActive(true);
                    is_checked = !is_checked;
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


void ui_checkbox::render() {}

void ui_checkbox::redraw() {

    int cur_index = vec_texture_index;
    np_object *temp_title_np;

    if ( is_pressed && my_style.has_pressed ) {
        temp_title_np = &my_style.pressed;
    } else if ( is_hovered && my_style.has_hover ) {
        temp_title_np = &my_style.hover;
    } else {
        temp_title_np = &my_style.normal;
    }

    temp_title_np->my_np[UI_9P_TL].addToVec(vec_texture,cur_index);
    temp_title_np->my_np[UI_9P_TM].addToVec(vec_texture,cur_index);
    temp_title_np->my_np[UI_9P_TR].addToVec(vec_texture,cur_index);
    temp_title_np->my_np[UI_9P_ML].addToVec(vec_texture,cur_index);
    temp_title_np->my_np[UI_9P_MM].addToVec(vec_texture,cur_index);
    temp_title_np->my_np[UI_9P_MR].addToVec(vec_texture,cur_index);
    temp_title_np->my_np[UI_9P_BL].addToVec(vec_texture,cur_index);
    temp_title_np->my_np[UI_9P_BM].addToVec(vec_texture,cur_index);
    temp_title_np->my_np[UI_9P_BR].addToVec(vec_texture,cur_index);

    for (int i=0; i < 36; i++) {
        vec_color->at(vec_color_index+3+4*i) = fade_percent;
    }

    checkmark.addToVec(vec_texture,cur_index);

    if ( is_checked || show_check_always )
        for (int i=0; i < 4; i++) vec_color->at(vec_color_index+3+36*4+4*i) = fade_percent;
    else
        for (int i=0; i < 4; i++) vec_color->at(vec_color_index+3+36*4+4*i) = 0;

}

void ui_checkbox::cook() {

    int cur_x = x;
    int cur_y = y;

    vec_texture_index = vec_texture->size();
    vec_vertex_index = vec_vertex->size();
    vec_color_index = vec_color->size();

    {
//upper left
    my_style.normal.my_np[UI_9P_TL].addToVec(vec_texture);
    //
    vec_vertex->push_back( cur_x );
    vec_vertex->push_back( cur_y - my_style.normal.my_np[UI_9P_TL].h );
    vec_vertex->push_back( cur_x );
    vec_vertex->push_back( cur_y );
    vec_vertex->push_back( cur_x + my_style.normal.my_np[UI_9P_TL].w );
    vec_vertex->push_back( cur_y );
    vec_vertex->push_back( cur_x + my_style.normal.my_np[UI_9P_TL].w );
    vec_vertex->push_back( cur_y - my_style.normal.my_np[UI_9P_TL].h );
    //
    cur_x += my_style.normal.my_np[UI_9P_TL].w;

//upper mid
    my_style.normal.my_np[UI_9P_TM].addToVec(vec_texture);
    //
    vec_vertex->push_back( cur_x );
    vec_vertex->push_back( cur_y - my_style.normal.my_np[UI_9P_TM].h );
    vec_vertex->push_back( cur_x );
    vec_vertex->push_back( cur_y );
    vec_vertex->push_back( cur_x + my_style.normal.my_np[UI_9P_TM].w + (w-my_style.min_x) );
    vec_vertex->push_back( cur_y );
    vec_vertex->push_back( cur_x + my_style.normal.my_np[UI_9P_TM].w + (w-my_style.min_x) );
    vec_vertex->push_back( cur_y - my_style.normal.my_np[UI_9P_TM].h );
    //
    cur_x += my_style.normal.my_np[UI_9P_TM].w + (w-my_style.min_x) ;

//upper right
    my_style.normal.my_np[UI_9P_TR].addToVec(vec_texture);
    //
    vec_vertex->push_back( cur_x );
    vec_vertex->push_back( cur_y - my_style.normal.my_np[UI_9P_TR].h );
    vec_vertex->push_back( cur_x );
    vec_vertex->push_back( cur_y );
    vec_vertex->push_back( cur_x + my_style.normal.my_np[UI_9P_TR].w );
    vec_vertex->push_back( cur_y );
    vec_vertex->push_back( cur_x + my_style.normal.my_np[UI_9P_TR].w );
    vec_vertex->push_back( cur_y - my_style.normal.my_np[UI_9P_TR].h );
    //
    cur_x = x;
    cur_y -= my_style.normal.my_np[UI_9P_TR].h;


//middle left
    my_style.normal.my_np[UI_9P_ML].addToVec(vec_texture);
    //
    vec_vertex->push_back( cur_x );
    vec_vertex->push_back( cur_y - my_style.normal.my_np[UI_9P_ML].h - (h-my_style.min_y) );
    vec_vertex->push_back( cur_x );
    vec_vertex->push_back( cur_y );
    vec_vertex->push_back( cur_x + my_style.normal.my_np[UI_9P_ML].w );
    vec_vertex->push_back( cur_y );
    vec_vertex->push_back( cur_x + my_style.normal.my_np[UI_9P_ML].w );
    vec_vertex->push_back( cur_y - my_style.normal.my_np[UI_9P_ML].h - (h-my_style.min_y) );
    //
    cur_x += my_style.normal.my_np[UI_9P_ML].w;

//middle mid
    my_style.normal.my_np[UI_9P_MM].addToVec(vec_texture);
    //
    vec_vertex->push_back( cur_x );
    vec_vertex->push_back( cur_y - my_style.normal.my_np[UI_9P_MM].h - (h-my_style.min_y) );
    vec_vertex->push_back( cur_x );
    vec_vertex->push_back( cur_y );
    vec_vertex->push_back( cur_x + my_style.normal.my_np[UI_9P_MM].w + (w-my_style.min_x) );
    vec_vertex->push_back( cur_y );
    vec_vertex->push_back( cur_x + my_style.normal.my_np[UI_9P_MM].w + (w-my_style.min_x) );
    vec_vertex->push_back( cur_y - my_style.normal.my_np[UI_9P_MM].h - (h-my_style.min_y) );
    //
    cur_x += my_style.normal.my_np[UI_9P_MM].w + (w-my_style.min_x) ;

//middle right
    my_style.normal.my_np[UI_9P_MR].addToVec(vec_texture);
    //
    vec_vertex->push_back( cur_x );
    vec_vertex->push_back( cur_y - my_style.normal.my_np[UI_9P_MR].h - (h-my_style.min_y) );
    vec_vertex->push_back( cur_x );
    vec_vertex->push_back( cur_y );
    vec_vertex->push_back( cur_x + my_style.normal.my_np[UI_9P_MR].w );
    vec_vertex->push_back( cur_y );
    vec_vertex->push_back( cur_x + my_style.normal.my_np[UI_9P_MR].w );
    vec_vertex->push_back( cur_y - my_style.normal.my_np[UI_9P_MR].h - (h-my_style.min_y) );
    //
    cur_x = x;
    cur_y -= my_style.normal.my_np[UI_9P_MR].h + (h-my_style.min_y);

//bottom left
    my_style.normal.my_np[UI_9P_BL].addToVec(vec_texture);
    //
    vec_vertex->push_back( cur_x );
    vec_vertex->push_back( cur_y - my_style.normal.my_np[UI_9P_BL].h );
    vec_vertex->push_back( cur_x );
    vec_vertex->push_back( cur_y );
    vec_vertex->push_back( cur_x + my_style.normal.my_np[UI_9P_BL].w );
    vec_vertex->push_back( cur_y );
    vec_vertex->push_back( cur_x + my_style.normal.my_np[UI_9P_BL].w );
    vec_vertex->push_back( cur_y - my_style.normal.my_np[UI_9P_BL].h );
    //
    cur_x += my_style.normal.my_np[UI_9P_BL].w;

//bottom mid
    my_style.normal.my_np[UI_9P_BM].addToVec(vec_texture);
    //
    vec_vertex->push_back( cur_x );
    vec_vertex->push_back( cur_y - my_style.normal.my_np[UI_9P_BM].h );
    vec_vertex->push_back( cur_x );
    vec_vertex->push_back( cur_y );
    vec_vertex->push_back( cur_x + my_style.normal.my_np[UI_9P_BM].w + (w-my_style.min_x) );
    vec_vertex->push_back( cur_y );
    vec_vertex->push_back( cur_x + my_style.normal.my_np[UI_9P_BM].w + (w-my_style.min_x) );
    vec_vertex->push_back( cur_y - my_style.normal.my_np[UI_9P_BM].h );
    //
    cur_x += my_style.normal.my_np[UI_9P_BM].w + (w-my_style.min_x) ;

//bottom right
    my_style.normal.my_np[UI_9P_BR].addToVec(vec_texture);
    //
    vec_vertex->push_back( cur_x );
    vec_vertex->push_back( cur_y - my_style.normal.my_np[UI_9P_BR].h );
    vec_vertex->push_back( cur_x );
    vec_vertex->push_back( cur_y );
    vec_vertex->push_back( cur_x + my_style.normal.my_np[UI_9P_BR].w );
    vec_vertex->push_back( cur_y );
    vec_vertex->push_back( cur_x + my_style.normal.my_np[UI_9P_BR].w );
    vec_vertex->push_back( cur_y - my_style.normal.my_np[UI_9P_BR].h );
    //
    cur_x = x;
    cur_y -= my_style.normal.my_np[UI_9P_BR].h;
    }

    for (int i=0; i < 36; i++) {
        vec_color->push_back(1.0);vec_color->push_back(1.0);vec_color->push_back(1.0);vec_color->push_back(1.0);
    }

    checkmark.addToVec(vec_texture);
    //
    vec_vertex->push_back( x + my_style.normal.my_np[UI_9P_TL].w );
    vec_vertex->push_back( y - h + my_style.normal.my_np[UI_9P_BL].h );
    vec_vertex->push_back( x + my_style.normal.my_np[UI_9P_TL].w );
    vec_vertex->push_back( y - my_style.normal.my_np[UI_9P_TL].h );
    vec_vertex->push_back( x + w - my_style.normal.my_np[UI_9P_TR].w );
    vec_vertex->push_back( y - my_style.normal.my_np[UI_9P_TR].h );
    vec_vertex->push_back( x + w - my_style.normal.my_np[UI_9P_TR].w );
    vec_vertex->push_back( y - h + my_style.normal.my_np[UI_9P_BR].h );

    for (int i=0; i < 4; i++) {
        vec_color->push_back(1.0);vec_color->push_back(1.0);vec_color->push_back(1.0);vec_color->push_back(1.0);
    }

    redraw();
}


/////////////////////////////////////////////////////////////////////////////////////////
//
void ui_checkbox::doFade(int fade_dir, int fade_length) {

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
void ui_checkbox::update() {

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
