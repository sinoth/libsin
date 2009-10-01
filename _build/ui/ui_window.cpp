
#include "sinui.h"

ui_window::ui_window() {
    setPointers(&real_vec_vertex, &real_vec_texture, &real_vec_color);
    show_closebutton = false;
    show_titlebar = false;
    can_resize = false;
}

void ui_window::enableCloseButton() {
    show_closebutton = true;
}

void ui_window::enableTitlebar() {
    show_titlebar = true;
    addChild(&titlebar);
    titlebar.setXY(0,0);
}

void ui_window::setInWH(int, int) {}
//    w=a+window_style.min_x-window_style.normal.my_np[UI_9P_MM].w;
//    h=b+window_style.min_y-window_style.normal.my_np[UI_9P_MM].h+titlebar_h; }

void ui_window::changeDims() {}

void ui_window::setThemeTextureID(GLuint in) { theme_texture = in; }


/////////////////////////////////////////////////////////////////////////////////////////
//
bool ui_window::eatKeyChar(int inchar, int instate) {
    for ( cit=children.begin(); cit != children.end(); cit++ ) {
        if ( (*cit)->eatKeyChar(inchar,instate) ) {
            return true;
        }
    }
    return false;
}

/////////////////////////////////////////////////////////////////////////////////////////
//
bool ui_window::eatKey(int inkey, int instate) {

    for ( cit=children.begin(); cit != children.end(); cit++ ) {
        if ( (*cit)->eatKey(inkey,instate) ) {
            return true;
        }
    }
    return false;
}


/////////////////////////////////////////////////////////////////////////////////////////
//
bool ui_window::eatMousePos(int inx, int iny) {

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
bool ui_window::eatMouseClick(int button, int state, int inx, int iny ) {

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

                //since we didn't click any children, unactive them
                if ( active_child != NULL ) { active_child->setActive(false); active_child = NULL; }


                return true;
            break;
        }
        break;
    }

    return false;
}


void ui_window::cook() {

    int cur_x = x;
    int cur_y = y - titlebar.h;

    vec_texture_index = vec_texture->size();
    vec_vertex_index = vec_vertex->size();
    vec_color_index = vec_color->size();


///////////////////////////////////////////////////////////////////
    {
//middle left
    window_style.normal.my_np[UI_9P_ML].addToVec(vec_texture);
    //
    vec_vertex->push_back( cur_x );
    vec_vertex->push_back( cur_y - window_style.normal.my_np[UI_9P_ML].h - (h-titlebar.h-window_style.min_y) );
    vec_vertex->push_back( cur_x );
    vec_vertex->push_back( cur_y );
    vec_vertex->push_back( cur_x + window_style.normal.my_np[UI_9P_ML].w );
    vec_vertex->push_back( cur_y );
    vec_vertex->push_back( cur_x + window_style.normal.my_np[UI_9P_ML].w );
    vec_vertex->push_back( cur_y - window_style.normal.my_np[UI_9P_ML].h - (h-titlebar.h-window_style.min_y) );
    //
    cur_x += window_style.normal.my_np[UI_9P_ML].w;

//middle mid
    window_style.normal.my_np[UI_9P_MM].addToVec(vec_texture);
    //
    vec_vertex->push_back( cur_x );
    vec_vertex->push_back( cur_y - window_style.normal.my_np[UI_9P_MM].h - (h-titlebar.h-window_style.min_y) );
    vec_vertex->push_back( cur_x );
    vec_vertex->push_back( cur_y );
    vec_vertex->push_back( cur_x + window_style.normal.my_np[UI_9P_MM].w + (w-window_style.min_x) );
    vec_vertex->push_back( cur_y );
    vec_vertex->push_back( cur_x + window_style.normal.my_np[UI_9P_MM].w + (w-window_style.min_x) );
    vec_vertex->push_back( cur_y - window_style.normal.my_np[UI_9P_MM].h - (h-titlebar.h-window_style.min_y) );
    //
    cur_x += window_style.normal.my_np[UI_9P_MM].w + (w-window_style.min_x) ;

//middle right
    window_style.normal.my_np[UI_9P_MR].addToVec(vec_texture);
    //
    vec_vertex->push_back( cur_x );
    vec_vertex->push_back( cur_y - window_style.normal.my_np[UI_9P_MR].h - (h-titlebar.h-window_style.min_y) );
    vec_vertex->push_back( cur_x );
    vec_vertex->push_back( cur_y );
    vec_vertex->push_back( cur_x + window_style.normal.my_np[UI_9P_MR].w );
    vec_vertex->push_back( cur_y );
    vec_vertex->push_back( cur_x + window_style.normal.my_np[UI_9P_MR].w );
    vec_vertex->push_back( cur_y - window_style.normal.my_np[UI_9P_MR].h - (h-titlebar.h-window_style.min_y) );
    //
    cur_x = x;
    cur_y -= window_style.normal.my_np[UI_9P_MR].h + (h-titlebar.h-window_style.min_y);

//bottom left
    window_style.normal.my_np[UI_9P_BL].addToVec(vec_texture);
    //
    vec_vertex->push_back( cur_x );
    vec_vertex->push_back( cur_y - window_style.normal.my_np[UI_9P_BL].h );
    vec_vertex->push_back( cur_x );
    vec_vertex->push_back( cur_y );
    vec_vertex->push_back( cur_x + window_style.normal.my_np[UI_9P_BL].w );
    vec_vertex->push_back( cur_y );
    vec_vertex->push_back( cur_x + window_style.normal.my_np[UI_9P_BL].w );
    vec_vertex->push_back( cur_y - window_style.normal.my_np[UI_9P_BL].h );
    //
    cur_x += window_style.normal.my_np[UI_9P_BL].w;

//bottom mid
    window_style.normal.my_np[UI_9P_BM].addToVec(vec_texture);
    //
    vec_vertex->push_back( cur_x );
    vec_vertex->push_back( cur_y - window_style.normal.my_np[UI_9P_BM].h );
    vec_vertex->push_back( cur_x );
    vec_vertex->push_back( cur_y );
    vec_vertex->push_back( cur_x + window_style.normal.my_np[UI_9P_BM].w + (w-window_style.min_x) );
    vec_vertex->push_back( cur_y );
    vec_vertex->push_back( cur_x + window_style.normal.my_np[UI_9P_BM].w + (w-window_style.min_x) );
    vec_vertex->push_back( cur_y - window_style.normal.my_np[UI_9P_BM].h );
    //
    cur_x += window_style.normal.my_np[UI_9P_BM].w + (w-window_style.min_x) ;

//bottom right
    window_style.normal.my_np[UI_9P_BR].addToVec(vec_texture);
    //
    vec_vertex->push_back( cur_x );
    vec_vertex->push_back( cur_y - window_style.normal.my_np[UI_9P_BR].h );
    vec_vertex->push_back( cur_x );
    vec_vertex->push_back( cur_y );
    vec_vertex->push_back( cur_x + window_style.normal.my_np[UI_9P_BR].w );
    vec_vertex->push_back( cur_y );
    vec_vertex->push_back( cur_x + window_style.normal.my_np[UI_9P_BR].w );
    vec_vertex->push_back( cur_y - window_style.normal.my_np[UI_9P_BR].h );
    //
    //cur_x = x;
    //cur_y -= window_style.normal.my_np[UI_9P_BR].h;
    }
///////////////////////////////////////////////////////////////////

    for (int i=0; i < 24; i++) {
        vec_color->push_back(1.0);vec_color->push_back(1.0);vec_color->push_back(1.0);vec_color->push_back(1.0);
    }


    if ( show_closebutton ) {
        cur_x = w - titlebar.h/2 - titlebar.my_style.normal.my_np[UI_9P_TR].w - 1;
        cur_y = titlebar.h/4;

        titlebar.addChild(&closebutton);
        closebutton.setXY(cur_x, cur_y);
        closebutton.setWH(titlebar.h/2, titlebar.h/2);
    }

    for ( cit = children.begin(); cit != children.end(); cit++ ) {
        (*cit)->cook();
    }
}


void ui_window::redraw() {

    int cur_index = vec_texture_index;
    np_object *temp_win_np;

    if ( is_hovered ) {
        temp_win_np = &window_style.hover;
    } else if ( is_pressed ) {
        temp_win_np = &window_style.pressed;
    } else {
        temp_win_np = &window_style.normal;
    }

    temp_win_np->my_np[UI_9P_ML].addToVec(vec_texture,cur_index);
    temp_win_np->my_np[UI_9P_MM].addToVec(vec_texture,cur_index);
    temp_win_np->my_np[UI_9P_MR].addToVec(vec_texture,cur_index);
    temp_win_np->my_np[UI_9P_BL].addToVec(vec_texture,cur_index);
    temp_win_np->my_np[UI_9P_BM].addToVec(vec_texture,cur_index);
    temp_win_np->my_np[UI_9P_BR].addToVec(vec_texture,cur_index);

    for (int i=0; i < 24; i++) {
        vec_color->at(vec_color_index+3+4*i) = fade_percent;
    }

}


void ui_window::render() {

    //if ( fade_percent == 0 ) return;

    glPushAttrib( GL_TEXTURE_BIT | GL_LIGHTING_BIT );
    glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);

    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, theme_texture);
    glColor4f(1.0,1.0,1.0,1.0);

      glEnableClientState(GL_VERTEX_ARRAY);
      glEnableClientState(GL_COLOR_ARRAY);
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);

      glVertexPointer(2, GL_FLOAT, 0, &real_vec_vertex[0] );
      glTexCoordPointer(2, GL_FLOAT, 0, &real_vec_texture[0] );
      glColorPointer(4, GL_FLOAT, 0, &real_vec_color[0] );
      glDrawArrays(GL_QUADS, 0, real_vec_vertex.size()/2 );

    //printf("ver:%d tex:%d col:%d\n", real_vec_vertex.size()/2, real_vec_texture.size()/2, real_vec_color.size()/2);

    glPopAttrib();
    glPopClientAttrib();

    if ( my_controller != NULL ) my_controller->render();

}


/////////////////////////////////////////////////////////////////////////////////////////
//
void ui_window::doFade(int fade_dir, int fade_length) {

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
void ui_window::update() {

    if ( fade_ticks ) {
        fade_ticks--;
        switch (fade_direction) {
            case UI_FADE_IN: fade_percent = 1.0 - (float)fade_ticks/(float)fade_ticks_max; break;
            case UI_FADE_OUT: fade_percent = (float)fade_ticks/(float)fade_ticks_max; break;
            default: break;
        }
        my_font.changeAlpha(fade_percent);

        redraw();

        if ( fade_ticks == 0 ) { setActive(false); }
    }

    //update the widgets
    for (cit=children.begin(); cit != children.end(); cit++) {
      (*cit)->update();
    }
}
