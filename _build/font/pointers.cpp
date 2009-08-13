
#include "sinfont.h"






///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////


font_list_pointerss::font_list_pointerss() : start_vertex(0),
                                             start_color(0),
                                             start_texture(0),
                                             active(true)
                                             { vec_vertex = NULL;
                                               vec_texture = NULL;
                                               vec_color = NULL;   }

void font_list_pointerss::setStart() { start_vertex = vec_vertex->size();
                                       start_texture = vec_texture->size();
                                       start_color = vec_color->size(); }

void font_list_pointerss::clearAll() { vec_vertex->clear();
                                       vec_texture->clear();
                                       vec_color->clear(); }

void font_list_pointerss::setPointers( std::vector<GLfloat> *invec, std::vector<GLfloat> *intex, std::vector<GLfloat> *incol ) {
        vec_vertex = invec; vec_texture = intex; vec_color = incol; }

void font_list_pointerss::newPointers() {
    vec_vertex = new std::vector<GLfloat>;
    vec_texture = new std::vector<GLfloat>;
    vec_color = new std::vector<GLfloat>;
}
