
#include "sinui.h"

tex_info_s::tex_info_s() { x_l=-1; w=-1; }
void tex_info_s::setTexXY(float inx_l,float iny_b,float inx_r,float iny_t) { x_l=inx_l; y_b=iny_b; x_r=inx_r; y_t=iny_t; }
void tex_info_s::setTex( float tx, float ty, float x, float y, float w, float h) {
        setTexXY( x/tx, (ty-y-h)/ty, (x+w)/tx, (ty-y)/ty ); }
void tex_info_s::setWH(short a, short b) { w=a; h=b; }
void tex_info_s::addToVec( std::vector<GLfloat> *in ) {
    in->push_back( x_l );
    in->push_back( y_b );
    in->push_back( x_l );
    in->push_back( y_t );
    in->push_back( x_r );
    in->push_back( y_t );
    in->push_back( x_r );
    in->push_back( y_b );
}
void tex_info_s::addToVec( std::vector<GLfloat> *in, int &index ) {
    in->at(index++) = x_l;
    in->at(index++) = y_b;
    in->at(index++) = x_l;
    in->at(index++) = y_t;
    in->at(index++) = x_r;
    in->at(index++) = y_t;
    in->at(index++) = x_r;
    in->at(index++) = y_b;
}

/////////////////////////////////////////////

void np_object_s::setNP(int type, float tx, float ty, float x, float y, float w, float h) {
    my_np[type].setTexXY( x/tx, (ty-y-h)/ty, (x+w)/tx, (ty-y)/ty );
    my_np[type].setWH(w, h);
}

/////////////////////////////////////////////
np_style_s::np_style_s() { has_active = false; has_hover = false; has_pressed = false; }
void np_style_s::cook() {
    min_x = normal.my_np[UI_9P_BL].w
          + normal.my_np[UI_9P_BM].w
          + normal.my_np[UI_9P_BR].w;
    min_y = normal.my_np[UI_9P_TL].h
          + normal.my_np[UI_9P_ML].h
          + normal.my_np[UI_9P_BL].h;

    font_x = normal.my_np[UI_9P_BL].w;
    font_y = normal.my_np[UI_9P_TL].h;

    font_w_off = normal.my_np[UI_9P_BL].w
               + normal.my_np[UI_9P_BR].w;
    font_h_off = normal.my_np[UI_9P_TL].h
               + normal.my_np[UI_9P_BL].h;

    if ( pressed.my_np[0].w != -1 ) has_pressed = true;
    if ( hover.my_np[0].w != -1 ) has_hover = true;
    if ( active.my_np[0].w != -1 ) has_active = true;
}
/////////////////////////////////////////////
tex_style_s::tex_style_s() {  has_active = false; has_hover = false; has_pressed = false; }
void tex_style_s::cook() {

    if ( pressed.x_l != -1 ) has_pressed = true;
    if ( hover.x_l != -1 ) has_hover = true;
    if ( active.x_l != -1 ) has_active = true;

}

/////////////////////////////////////////////////////////////////////////////////////////
//

mouseover_s::mouseover_s() : X1(0), Y1(0), X2(0), Y2(0) {}
bool mouseover_s::isMouseover(int inx,int iny) {
    if ( inx >= X1 && inx < X2 && iny > Y1 && iny <= Y2 ) return true;
    else return false; }
void mouseover_s::set(int inx1, int iny1, int inx2, int iny2 ) { X1=inx1; Y1=iny1; X2=inx2; Y2=iny2; }
void mouseover_s::moveBy(int xoffset, int yoffset) { X1+=xoffset; X2+=xoffset; Y1+=yoffset; Y2+=yoffset; }
//void mouseover_s::scaleXY(float xscale, float yscale) { X1*=xscale; X2+=xscale; Y1*=yscale; Y2*=yscale; }

