
#include "sinfont.h"

//////////////////////////////////////////////////////////////////////////
//
freetype_font_controller::freetype_font_controller() {}

void freetype_font_controller::registerObject(font_object* in) { registerObject(in,FONT_HINT_STATIC); }
void freetype_font_controller::registerObject(font_object* in, int hint) { registerObject(in,hint,-1); }
void freetype_font_controller::registerObject(font_object* in, int hint, int group) {

    in->setHint(hint);

    switch ( hint ) {
        case FONT_HINT_STATIC:
            if ( render_map.find(in->my_font->getTextureID()) == render_map.end() ) {
                render_map[in->my_font->getTextureID()].push_back(font_list_pointers());
                render_map[in->my_font->getTextureID()].front().newPointers();
                in->my_pointers = render_map[in->my_font->getTextureID()].front();
                in->my_pointers.parent = &render_map[in->my_font->getTextureID()].front();
                render_map[in->my_font->getTextureID()].front().last_font_object = in;
            } else {
                render_map[in->my_font->getTextureID()].front().setStart();
                in->my_pointers = render_map[in->my_font->getTextureID()].front();
                in->my_pointers.parent = &render_map[in->my_font->getTextureID()].front();
                render_map[in->my_font->getTextureID()].front().last_font_object = in;
            }
            break;
        case FONT_HINT_DYNAMIC:
            if ( group == -1 ) {
                render_map[in->my_font->getTextureID()].push_back(font_list_pointers());
                render_map[in->my_font->getTextureID()].back().newPointers();
                in->my_pointers = render_map[in->my_font->getTextureID()].back();
                in->my_pointers.parent = &render_map[in->my_font->getTextureID()].back();
                render_map[in->my_font->getTextureID()].back().last_font_object = in;
                int position = -1;
                for ( lit = render_map[in->my_font->getTextureID()].begin(); lit != render_map[in->my_font->getTextureID()].end(); lit++,position++ );
                in->setGroup(position);
                //printf("setting group position to %d\n", position);
            } else {
                lit = render_map[in->my_font->getTextureID()].begin();
                for (int i=0; i<group; i++ ) lit++;
                in->my_pointers = (*lit);
                in->my_pointers.parent = &(*lit);
                (*lit).last_font_object = in;
            }
            break;
    }

    //in->cook();
}

void freetype_font_controller::render() {

    glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);
    glPushAttrib( GL_TEXTURE_BIT | GL_LIGHTING_BIT | GL_COLOR_BUFFER_BIT );

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glDisable(GL_LIGHTING);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    for ( it=render_map.begin(); it != render_map.end(); it++ ) {

        glBindTexture( GL_TEXTURE_2D, (*it).first );

        //for every font in the list, we have to go through every set of pointers
        for (lit=(*it).second.begin(); lit != (*it).second.end(); lit++ ) {
            if ( !(*lit).active ) continue;

            glVertexPointer(3, GL_FLOAT, 0, &(*lit).vec_vertex->at(0) );
            glTexCoordPointer(2, GL_FLOAT, 0, &(*lit).vec_texture->at(0) );
            glColorPointer(4, GL_FLOAT, 0, &(*lit).vec_color->at(0) );
            glDrawArrays(GL_TRIANGLE_STRIP, 0, (*lit).vec_vertex->size()/3 );
            //glInterleavedArrays(GL_T2F_V3F,0,&(*lit)->vec_vertex->at(0) );
        }

    }

    glPopAttrib();
    glPopClientAttrib();

}

//////////////////////////////////////////////////////////////////////////
//
void freetype_font_controller::translate(int inx, int iny) {
    static std::vector<GLfloat>::iterator vit;

    for ( it=render_map.begin(); it != render_map.end(); it++ ) {
        for (lit=(*it).second.begin(); lit != (*it).second.end(); lit++ ) {
            for (vit=(*lit).vec_vertex->begin(); vit != (*lit).vec_vertex->end(); vit++) {
                (*vit) += inx;
                vit++;
                (*vit) += iny;
                vit++;
            }
        }
    }

}



//////////////////////////////////////////////////////////////////////////
//
void freetype_font_controller::storeTexID() {

    render_map_lookup.clear();

    //for every entry in the render map, we need to store just
    //one font_object that points to it
    for ( it=render_map.begin(); it != render_map.end(); it++ ) {
        //if ( render_map_lookup.find((*it).first) == render_map_lookup.end() )
            render_map_lookup[(*it).first] = (*it).second.front().last_font_object;
    }

}


//////////////////////////////////////////////////////////////////////////
//
void freetype_font_controller::restoreTexID() {

    render_map_backup = render_map;
    render_map.clear();

    //for every entry in the render map, we need to store just
    //one font_object that points to it
    for ( it=render_map_backup.begin(); it != render_map_backup.end(); it++ ) {
        //if ( render_map_lookup.find((*it).first) == render_map_lookup.end() )
        //    render_map_lookup[(*it).first] = (*it).second.front().last_font_object;
        render_map[render_map_lookup[(*it).first]->my_font->getTextureID()] = (*it).second;
    }

    render_map_backup.clear();
    render_map_lookup.clear();

}


/*
//////////////////////////////////////////////////////////////////////////
//
void freetype_font_controller::scaleXY(float inx, float iny) {
    static std::vector<GLfloat>::iterator vit;

    for ( it=render_map.begin(); it != render_map.end(); it++ ) {
        for (lit=(*it).second.begin(); lit != (*it).second.end(); lit++ ) {
            for (vit=(*lit).vec_vertex->begin(); vit != (*lit).vec_vertex->end(); vit++) {
                (*vit) *= inx;
                vit++;
                (*vit) *= iny;
                vit++;
            }
        }
    }

}
*/
