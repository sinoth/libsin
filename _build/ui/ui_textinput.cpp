#include <sintimer.h>
#include "sinui.h"


ui_textinput::ui_textinput() {
    cursor_x_size = 2;
    cursor_y_size_offset = 4;
    cursor_x_offset = 0;
    max_text_length = 0;
    cursor_is_blinking = false;

    doing_backspace = false;
    doing_backspace_wait = false;
    backspace_delay = 300;
    backspace_timer.initMS(backspace_delay);
    backspace_repeat_delay = 70;
    backspace_repeat_timer.initMS(backspace_repeat_delay);

    doing_keyrepeat = false;
    doing_keyrepeat_wait = false;
    keyrepeat_delay = 300;
    keyrepeat_timer.initMS(keyrepeat_delay);
    keyrepeat_repeat = 100;
    keyrepeat_timer_repeat.initMS(keyrepeat_repeat);
}

void ui_textinput::changeDims() {
    my_font.setXY(x+my_style.font_x, y-1);
    my_font.setMaxWH(w-my_style.font_w_off, h-2);
}

void ui_textinput::setMaxTextLength(unsigned int in) { max_text_length = in; }

void ui_textinput::setText(const char* in) {
    my_text = in;
    my_font.setText(in);
}

char *ui_textinput::getText() {
    char* temp_char = new char[my_text.size()+1];
    strcpy(temp_char, my_text.c_str() );
    return (char*)&(my_style.font_x);
    return temp_char;
}

void ui_textinput::addChar(char in) {
    if ( my_text.size() <= max_text_length ) {
        my_text += in;
        my_font.setText(my_text.c_str());
        cursor_x_offset = my_font.getLength(my_text.c_str(), 1.0);
        //vec_vertex cursor_x_offset + x + my_style.font_x
        vec_vertex->at(vec_vertex_index+72+0) = cursor_x_offset + x + my_style.font_x;
        vec_vertex->at(vec_vertex_index+72+2) = cursor_x_offset + x + my_style.font_x;
        vec_vertex->at(vec_vertex_index+72+4) = cursor_x_offset + x + my_style.font_x + cursor_x_size;
        vec_vertex->at(vec_vertex_index+72+6) = cursor_x_offset + x + my_style.font_x + cursor_x_size;
        my_font.cook();
    }
}

void ui_textinput::deleteChar() {
    if ( my_text.size() > 0 ) {
        my_text.erase(my_text.size()-1);
        my_font.setText(my_text.c_str());
        cursor_x_offset = my_font.getLength(my_text.c_str(), 1.0);
        vec_vertex->at(vec_vertex_index+72+0) = cursor_x_offset + x + my_style.font_x;
        vec_vertex->at(vec_vertex_index+72+2) = cursor_x_offset + x + my_style.font_x;
        vec_vertex->at(vec_vertex_index+72+4) = cursor_x_offset + x + my_style.font_x + cursor_x_size;
        vec_vertex->at(vec_vertex_index+72+6) = cursor_x_offset + x + my_style.font_x + cursor_x_size;
        my_font.cook();
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
//
bool ui_textinput::eatKeyChar(int inkey, int instate) {
    if ( is_active ) {
        switch ( instate ) {
            case GLFW_PRESS:
                addChar((char)inkey);
                last_char = inkey;
                doing_keyrepeat_wait = true;
                keyrepeat_timer.reset();
                return true;
                break;

            case GLFW_RELEASE:
                doing_keyrepeat = false;
                doing_keyrepeat_wait = false;
                return true;
                break;
        }
    }

    return false;
}


/////////////////////////////////////////////////////////////////////////////////////////
//
bool ui_textinput::eatKey(int inkey, int instate) {

    if ( custom_key_callback != NULL )
        if ( custom_key_callback(inkey,instate) ) return true;

    switch (instate) {
        case GLFW_PRESS:
            switch ( inkey ) {
                case GLFW_KEY_BACKSPACE:
                    doing_backspace_wait = true;
                    doing_backspace = false;
                    backspace_timer.reset();
                    return true;
                    break;
            }
            break;

        case GLFW_RELEASE:
            switch ( inkey ) {
                case GLFW_KEY_BACKSPACE:
                    //if we didn't wait long enough, assume a single delete
                    if ( !doing_backspace ) { deleteChar(); }
                    doing_backspace_wait = false;
                    doing_backspace = false;
                    return true;
                    break;
                case GLFW_KEY_ESC:
                case GLFW_KEY_ENTER:
                    setActive(false);
                    return true;
                    break;
            }
            break;
    }

    return false;
}


/////////////////////////////////////////////////////////////////////////////////////////
//

bool ui_textinput::eatMousePos(int inx, int iny) {

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
bool ui_textinput::eatMouseClick(int button, int state, int inx, int iny ) {

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
                            //active_child->setActive(true);
                        }

                        setPressed(false);
                        return true;
                    }
                }

                if ( is_dragging ) is_dragging = false;
                if ( can_drag_parent ) stopParentDrag();
                if ( is_pressed ) setActive(true);
                setPressed(false);

                return true;
            break;
        }
        break;
    }

    return false;
}


void ui_textinput::setStyle( np_style &in ) { my_style = in; }
void ui_textinput::setCursorSize(int in) { cursor_x_size = in; }
void ui_textinput::setInWH(int,int) {}
void ui_textinput::render() {}

void ui_textinput::redraw() {

    int cur_index = vec_texture_index;
    np_object *temp_title_np;

    if ( is_pressed && my_style.has_pressed ) {
        temp_title_np = &my_style.pressed;
    } else if ( is_active && my_style.has_active ) {
        temp_title_np = &my_style.active;
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

    //36 + 4 for cursor
    for (int i=0; i < 36; i++) {
        vec_color->at(vec_color_index+3+4*i) = fade_percent;
    }
    for (int i=36; i < 40; i++) {
        if ( is_active && cursor_is_blinking ) {
            vec_color->at(vec_color_index+3+4*i) = fade_percent;
        } else {
            vec_color->at(vec_color_index+3+4*i) = 0;
        }
    }

}

void ui_textinput::cook() {

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


    //the cursor
    cursor_x_offset = my_font.getLength(my_text.c_str(), 1.0);
    vec_vertex->push_back( cursor_x_offset + x + my_style.font_x );
    vec_vertex->push_back( y - h + cursor_y_size_offset );
    vec_vertex->push_back( cursor_x_offset + x + my_style.font_x );
    vec_vertex->push_back( y - cursor_y_size_offset);
    vec_vertex->push_back( cursor_x_offset + x + my_style.font_x + cursor_x_size );
    vec_vertex->push_back( y - cursor_y_size_offset);
    vec_vertex->push_back( cursor_x_offset + x + my_style.font_x + cursor_x_size );
    vec_vertex->push_back( y - h + cursor_y_size_offset );



    for (int i=36; i < 40; i++) {
        vec_color->push_back(0.0);vec_color->push_back(0.0);vec_color->push_back(0.0);vec_color->push_back(1.0);
        vec_texture->push_back(0); vec_texture->push_back(0);
    }

    for ( cit = children.begin(); cit != children.end(); cit++ ) {
        (*cit)->cook();
    }


}



/////////////////////////////////////////////////////////////////////////////////////////
//
void ui_textinput::doFade(int fade_dir, int fade_length) {

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
void ui_textinput::update() {

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

    //backspace routine
    if ( doing_backspace_wait && !doing_backspace && backspace_timer.needUpdateNoCarry() ) {
        doing_backspace = true;
        backspace_repeat_timer.reset();
    } else if ( doing_backspace && backspace_repeat_timer.needUpdateNoCarry() ) {
        deleteChar();
    }

    //keyrepeat routine
    if ( doing_keyrepeat_wait && !doing_keyrepeat && keyrepeat_timer.needUpdateNoCarry() ) {
        doing_keyrepeat = true;
        keyrepeat_timer_repeat.reset();
    } else if ( doing_keyrepeat && keyrepeat_timer_repeat.needUpdateNoCarry() ) {
        addChar(last_char);
    }

    static sinTimer blink_timer(3);
    //if we're active, do the blinky cursor!
    if ( is_active ) {
        if ( blink_timer.needUpdateNoCarry() ) cursor_is_blinking^=1;

        for (int i=36; i < 40; i++) {
            if ( cursor_is_blinking ) {
                vec_color->at(vec_color_index+3+4*i) = fade_percent;
            } else {
                vec_color->at(vec_color_index+3+4*i) = 0;
            }
        }

    } else {
        for (int i=36; i < 40; i++)
            vec_color->at(vec_color_index+3+4*i) = 0;
    }

    //update the widgets
    for (cit=children.begin(); cit != children.end(); cit++) {
      (*cit)->update();
    }
}

