#include "sinui.h"

/////////////////////////////////////////////////////////////////////////////////////////
//
SinUI::SinUI() {
    activeWindow = NULL;
    hoverWindow = NULL;
    lastClickedWindow = NULL;
    draggingWindow = NULL;
}


ui_window* SinUI::getActiveWindow() { return activeWindow; }


/////////////////////////////////////////////////////////////////////////////////////////
//
ui_window* SinUI::addWindow(const char *inname ) {

    ui_window *new_window = new ui_window;
    new_window->my_name = inname;

    if ( window_map.find(inname) != window_map.end() ) {
        printf("ERROR: Insert window to UI with name %s already exists!", inname);
        return NULL;
    }

    window_map[inname] = new_window;
    window_list.push_front(new_window);
    activeWindow = new_window;

    return new_window;
}


/////////////////////////////////////////////////////////////////////////////////////////
//
bool SinUI::insertKeyChar(int character, int state ) {

    for (it=window_list.begin(); it != window_list.end(); it++) {
        if ( (*it)->eatKeyChar(character,state) )
            return true;
    }

    return false;
}

/////////////////////////////////////////////////////////////////////////////////////////
//
bool SinUI::insertKey(int key, int state ) {

    for (it=window_list.begin(); it != window_list.end(); it++) {
        if ( (*it)->is_visible && (*it)->eatKey(key,state) )
            return true;
    }

    return false;
}

/////////////////////////////////////////////////////////////////////////////////////////
//
bool SinUI::insertMousePos(int x, int y ) {

    if ( draggingWindow != NULL ) {
        draggingWindow->parentDrag(x,y);
        return true;
    }

    for (it=window_list.begin(); it != window_list.end(); it++) {
        if ( (*it)->eatMousePos(x,y) ) {
            if (  hoverWindow != (*it) && hoverWindow != NULL ) {
                hoverWindow->is_hovered = false;
                hoverWindow->is_pressed = false;
            }
            hoverWindow = (*it);
            return true;
        }
    }

    //not over anyone, so send unhover/unactive to everyone
    if ( hoverWindow != NULL ) {
        hoverWindow->setHover(false);
        hoverWindow = NULL;
    }

    return false;
}


/////////////////////////////////////////////////////////////////////////////////////////
//
bool SinUI::insertMouseClick(int button, int state, int inx, int iny ) {

    switch ( state ) {
        case GLFW_PRESS:
        switch ( button ) {
            case GLFW_MOUSE_BUTTON_LEFT:
                for (it=window_list.begin(); it != window_list.end(); it++) {
                    if ( ((*it)->fade_ticks == 0 || (*it)->fade_direction == UI_FADE_IN)
                         && (*it)->eatMouseClick(button,state,inx,iny) ) {
                        //bringToFront((*it));

                        if ( activeWindow != NULL && activeWindow != (*it) ) activeWindow->setActive(false);
                        activeWindow = (*it);

                        if ( (*it)->is_dragging ) {
                            draggingWindow = (*it);
                        }

                        lastClickedWindow = (*it);

                        return true;
                    }
                }
                lastClickedWindow = NULL;
            break;
        }
        break;

        case GLFW_RELEASE:
        switch ( button ) {
            case GLFW_MOUSE_BUTTON_LEFT:
                draggingWindow = NULL;

                for (it=window_list.begin(); it != window_list.end(); it++) {
                    if ( ((*it)->fade_ticks == 0 || (*it)->fade_direction == UI_FADE_IN)
                         && (*it)->eatMouseClick(button,state,inx,iny) ) {

                        //if ( (*it)->is_active ) {
                        //    if ( activeWindow != NULL && activeWindow != (*it) ) activeWindow->setActive(false);
                        //    activeWindow = (*it);
                        //    activeWindow->setActive(true);
                        //}

                        return true;
                    }
                }

                //released on nothing, unac tive if possible
                if ( activeWindow != NULL && lastClickedWindow == NULL ) {
                    //printf("* UI: clicked nothing\n");
                    activeWindow->setActive(false);
                    activeWindow = NULL;
                }
            break;
        }
        break;
    }

    return false;
}


/////////////////////////////////////////////////////////////////////////////////////////
//
void SinUI::render() {

    //we render in reverse order so the first is on top
    for (rit=window_list.rbegin(); rit != window_list.rend(); rit++) {
        if ( (*rit)->is_visible )
            (*rit)->render();
    }

}

/////////////////////////////////////////////////////////////////////////////////////////
//
void SinUI::update() {
  for (it=window_list.begin(); it != window_list.end(); it++) {
    (*it)->update();
  }
}


/////////////////////////////////////////////////////////////////////////////////////////
//
ui_window* SinUI::getWindow(const char* in) {

    if ( window_map.find(in) == window_map.end() ) {
        printf("* UI - ERROR: cannot find window with name [%s]\n", in);
        return NULL;
    }

    return window_map.find(in)->second;
}


/////////////////////////////////////////////////////////////////////////////////////////
//
ui_base* SinUI::getWidget(const char* in) {

    if ( widget_map.find(in) == widget_map.end() ) {
        printf("* UI - ERROR: cannot find widget with name [%s]\n", in);
        return NULL;
    }

    return widget_map.find(in)->second;
}

/////////////////////////////////////////////////////////////////////////////////////////
//
ui_base* SinUI::addWidget(const char *inname, int intype ) {

    ui_base *new_widget;

    switch (intype) {
        case UI_WIDGET_BASE:        new_widget = new ui_base; break;
        case UI_WIDGET_BUTTON:      new_widget = new ui_button; break;
        case UI_WIDGET_BUTTON_NP:   new_widget = new ui_button_np; break;
        case UI_WIDGET_CHECKBOX:    new_widget = new ui_checkbox; break;
        case UI_WIDGET_LABEL:       new_widget = new ui_label; break;
        //case UI_WIDGET_LISTBOX:   new_widget = new ui_listbox; break;
        case UI_WIDGET_TEXTINPUT:   new_widget = new ui_textinput; break;
        default:
            new_widget = NULL;
            printf("* ERROR: addWidget called with invalid type\n"); break;
    }

    new_widget->my_name = inname;

    if ( widget_map.find(inname) != widget_map.end() ) {
        printf("ERROR: Insert widget to [%s] window with name [%s] already exists!", activeWindow->my_name.c_str(), inname);
        delete new_widget;
        return NULL;
    }

    widget_map[inname] = new_widget;
    activeWindow->addChild( new_widget );
    new_widget->setParent( activeWindow );

    printf("* UI - [%s] adding widget [%s]\n", activeWindow->my_name.c_str(), inname);

    return new_widget;

}


/////////////////////////////////////////////////////////////////////////////////////////
//
void SinUI::bringToFront(ui_window* in) {

    bool found = false;


    for (it=window_list.begin(); found == false && it != window_list.end(); it++) {
        if ( (*it) == in ) { found = true; break; }
    }

    if ( found ) {
        window_list.erase(it);
        window_list.push_front(in);
        activeWindow = in;
    }

}



/////////////////////////////////////////////////////////////////////////////////////////
//
void SinUI::globalResize(int x_old, int y_old, int x_new, int y_new ) {

    for (it=window_list.begin(); it != window_list.end(); it++) {

        //printf(" %f \n", (float)((*it)->y-(*it)->h/2)/(float)y_old * (float)y_new - ((*it)->y-(*it)->h/2) );

        (*it)->moveBy( (float)((*it)->x+(*it)->w/2)/(float)x_old * (float)x_new - ((*it)->x+(*it)->w/2) ,
                       (float)((*it)->y-(*it)->h/2)/(float)y_old * (float)y_new - ((*it)->y-(*it)->h/2) );

    }

}
