
#include "sinfont.h"


glyph_info::glyph_info( char a, char b, float c, float d, float e, float f, short int g, short int h, short int i, char j ) {
x_offset = a; y_offset = b; x_left = c; x_right = d; y_top = e; y_bottom = f; advance = g; height = h; width = i; character = j;  }


freetype_font::freetype_font()
  : origin_topleft(false),
    r(0),g(0),b(0),a(1),
    max_height(0)
    { my_list = NULL; }

int freetype_font::init( const char *infile, int insize ) {

    FT_Library    library;
    FT_Face       face;
    FT_GlyphSlot  slot;
    FT_Bitmap*    bitmap;

    int error;
    int x_width;
    int tex_width;
    int tex_height;
    int pen_x;
    int pen_y;
    int pen_offset;
    int max_real_height;
    int min_real_height;
    int max_boxheight;
    unsigned int glyph_index;

    my_languages.populate_list();

    printf("* INIT: Trying to init FreeType... ");

    //my_matrix = new glyph_matrix[95];

    error = FT_Init_FreeType( &library );
    if ( error )
        { printf("ERROR loading FreeType\n"); }

    error = FT_New_Face( library, infile, 0, &face );
    if ( error == FT_Err_Unknown_File_Format )
        { printf("ERROR in Freetype FT_New_Face - font found but not supported\n"); }
    else if ( error )
        { printf("ERROR in Freetype FT_New_Face - font not found\n"); }

    error = FT_Set_Pixel_Sizes( face, 0, insize );
    if ( error )
        { printf("ERROR in FreeType - FT_Set_Pixel_Sizes\n"); }

    slot = face->glyph;

    //printf("past main inits... ");

    //find the width of texture
    x_width = 0; max_real_height = 0; min_real_height = 0;
    //for ( int char_offset = 0; char_offset < 65534; char_offset++ ) {
    for ( my_languages.it = my_languages.valid_chars.begin(); my_languages.it != my_languages.valid_chars.end(); my_languages.it++ ) {
        glyph_index = FT_Get_Char_Index( face, (*my_languages.it) );
        if ( glyph_index != 0 ) {
            error = FT_Load_Glyph( face, glyph_index, FT_LOAD_RENDER );
            if ( error ) { printf("ERROR in FreeType - FT_Load_Glyph\n"); }
            bitmap = &slot->bitmap;

            bitmap = (FT_Bitmap*)&slot->bitmap;
            x_width += ((FT_Bitmap*)&slot->bitmap)->width + 1 ;

            if ( bitmap->rows > max_height ) { max_height = bitmap->rows; }

            if ( slot->bitmap_top > max_real_height ) { max_real_height = slot->bitmap_top; }
            if ( slot->bitmap_top - bitmap->rows < min_real_height ) { min_real_height = slot->bitmap_top - bitmap->rows; }
        }

    }

    max_boxheight = max_real_height - min_real_height;

    //printf("font - max_height: %d, max_real_height: %d, min_real_height:  ,max_box: %d%d\n",max_height, max_real_height, min_real_height, max_boxheight);

    // ok... start by finding area that will hold the guessed area, start at 32x32 and go up (step up each by a power)
    // once you get required area, do a dry run of placement to see if it will fit
    // if not, increase the Y by a power and be done with it

    tex_width = 32; tex_height = 32;

    while ( tex_width*tex_height < x_width * (max_height+1) ) {
        tex_width <<= 1;
        if ( tex_width*tex_height < x_width * (max_height+1) )
            tex_height <<= 1;
    }

    //printf("\naccum width: %d, max height: %d\n", x_width, max_height);
    //printf("guessed area: %d\n", x_width * (max_height+1) );
    //printf("calc area: %d [%d x %d]\n", tex_width * tex_height, tex_width, tex_height );

    //dry run here to make sure it all fits
    pen_x=0; pen_y=0;
    for ( my_languages.it = my_languages.valid_chars.begin(); my_languages.it != my_languages.valid_chars.end(); my_languages.it++ ) {
        glyph_index = FT_Get_Char_Index( face, (*my_languages.it) );
        if ( glyph_index != 0 ) {
            error = FT_Load_Glyph( face, glyph_index, FT_LOAD_RENDER );
            if ( error ) { printf("ERROR in FreeType - FT_Load_Glyph\n"); }
            bitmap = &slot->bitmap;
                if ( (pen_x + bitmap->width) > (tex_width-1) ) {
                    pen_y += max_height+1; pen_x = 0;
                }
                pen_x += bitmap->width + 1;
        }
    }

    if ( pen_y >= tex_height - max_height ) {
        //texture was too small, need to make bigger
        printf("eep.. ");
        if ( tex_width == tex_height )
            tex_width <<= 1;
        else
            tex_height <<= 1;
    }

    printf("[%dx%d].. ", tex_width, tex_height);

    atlas_data = new GLubyte[ 2 * tex_width * tex_height ];
    for (int i=0; i < tex_height*tex_width; i++) {
        //default alpha value
        atlas_data[i] = 0; }

    pen_x=0; pen_y=0;
//    for ( int char_offset = 0; char_offset <= 94; char_offset++ ) {
//        error = FT_Load_Char( face, 32+char_offset, FT_LOAD_RENDER );
    for ( my_languages.it = my_languages.valid_chars.begin(); my_languages.it != my_languages.valid_chars.end(); my_languages.it++ ) {
        glyph_index = FT_Get_Char_Index( face, (*my_languages.it) );
        if ( glyph_index != 0 ) {
            error = FT_Load_Glyph( face, glyph_index, FT_LOAD_RENDER );
            if ( error ) { printf("ERROR in FreeType - FT_Load_Glyph\n"); }
            bitmap = &slot->bitmap;

            if ( (pen_x + bitmap->width) > (tex_width-1) ) {
                pen_y += max_height+1; pen_x = 0;
            }

            pen_offset = pen_x + ((tex_height-max_height-pen_y) * tex_width);
            for(int j=0; j <bitmap->rows; j++) {
                for(int i=0; i < bitmap->width; i++){
                    atlas_data[(pen_offset+i+(bitmap->rows-1-j)*tex_width)] = bitmap->buffer[i + bitmap->width*j];
                }
            }

            glyphs[(*my_languages.it)] = glyph_info( slot->bitmap_left,               // x_offset
                                          //slot->bitmap_top - max_height, // y_offset
                                          //slot->bitmap_top - max_height + (max_height-bitmap->rows), // y_offset
                                          -max_real_height + slot->bitmap_top, // y_offset
                                          (float)(pen_x)/(float)tex_width, //left
                                          (float)(pen_x+bitmap->width)/(float)tex_width, //right
                                          //1.0-(float)(pen_y + (max_height-bitmap->rows))/(float)tex_height, //top
                                          1.0-(float)(pen_y + (max_height-bitmap->rows))/(float)tex_height, //top
                                          1.0-(float)(pen_y + max_height )/(float)tex_height, //bottom
                                          slot->advance.x >> 6,            // advance
                                          bitmap->rows,                  // height
                                          //max_height,                      // height
                                          bitmap->width,                   // width
                                          (*my_languages.it)               //ascii char
                                          ) ;

            pen_x += bitmap->width + 1;

        }
    }


    glGenTextures( 1, &atlas_texture );
    glBindTexture( GL_TEXTURE_2D, atlas_texture );

    //glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

    // Use trilinear interpolation for minification
    //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR );
    // Use bilinear interpolation for magnification
    //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR );

    //glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

    // the texture wraps over at the edges (repeat)
    //glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    //glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

	//glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0,
		//GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, atlas_data );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_ALPHA, tex_width, tex_height, 0,
		GL_ALPHA, GL_UNSIGNED_BYTE, atlas_data );

	delete [] atlas_data;

    max_height = max_boxheight;

    newline_advance = face->size->metrics.height >> 6;
    printf("\nnewline_advance: %d\n", newline_advance);

    printf("done.\n");

    return 0;

}



int freetype_font::createText( const char *intext, std::vector<GLfloat> *invec, float inscale, int inx, int iny, int inmax ) {
static int a, i_glyph;
static float x_left, x_right;
static float y_top, y_bottom;
static int width, height;
static int x_off, y_off;
static int phrase_len;
static int old_inx;
static int keep_inx;
static int ver_count;

static std::vector<GLfloat> *vec_vertex;
static std::vector<GLfloat> *vec_texture;
static std::vector<GLfloat> *vec_color;

old_inx = inx;
phrase_len = strlen(intext);
ver_count = 0;

//invec->clear();
//uses GL_T2F_V3F

  if ( invec == NULL ) {
      vec_vertex = my_list->vec_vertex;
      vec_texture = my_list->vec_texture;
      vec_color = my_list->vec_color;
      //vec_vertex = my_list->vec_vertex;
      //vec_texture = my_list->vec_vertex;
      //vec_color = NULL;

  } else {
      vec_vertex = invec;
      vec_texture = invec;
      vec_color = NULL;
  }

  if ( !origin_topleft ) {

    for ( a=0; a < phrase_len; a++ ) {

        //ABCDEF+ FGGH + GHIJKL

        i_glyph = intext[a];
        //printf("Operating on glyph %d\n",i_glyph);

        //check to see if given value is even in the map.  if not, continue to next char
        mit = glyphs.find(i_glyph);
        if ( mit == glyphs.end() ) continue;

        if ( !inmax || (inx+(int)(mit->second.advance * inscale )-old_inx) <= inmax ) {

        x_left = mit->second.x_left;  x_right = mit->second.x_right;
        y_top = mit->second.y_top;    y_bottom = mit->second.y_bottom;
        height = mit->second.height;  width = mit->second.width;
        x_off = mit->second.x_offset; y_off = mit->second.y_offset;

        width  = (int)((float)width  * inscale + 0.5);
        height = (int)((float)height * inscale + 0.5);
        x_off  = (int)((float)x_off  * inscale + 0.5);
        y_off  = (int)((float)y_off  * inscale + 0.5);

        if ( a==0 && vec_vertex->size()!=0 ) {

            vec_texture->push_back(x_left);
            vec_texture->push_back(y_top);
            //####//
            vec_vertex->push_back( inx + x_off );
            vec_vertex->push_back( iny + y_off );
            vec_vertex->push_back( 0 );
            ver_count++;

            vec_texture->push_back(x_left);
            vec_texture->push_back(y_top);
            //####//
            vec_vertex->push_back( inx + x_off );
            vec_vertex->push_back( iny + y_off );
            vec_vertex->push_back( 0 );
            ver_count++;

            vec_texture->push_back( x_right );
            vec_texture->push_back( y_top );
            //####//
            vec_vertex->push_back( inx + x_off + width );
            vec_vertex->push_back( iny + y_off );
            vec_vertex->push_back( 0 );
            ver_count++;
        }

        vec_texture->push_back(x_left);
        vec_texture->push_back(y_top);
        //####//
        vec_vertex->push_back( inx + x_off );
        vec_vertex->push_back( iny + y_off );
        vec_vertex->push_back( 0 );
        ver_count++;

        vec_texture->push_back( x_right );
        vec_texture->push_back( y_top );
        //####//
        vec_vertex->push_back( inx + x_off + width );
        vec_vertex->push_back( iny + y_off );
        vec_vertex->push_back( 0 );
        ver_count++;

        vec_texture->push_back( x_left );
        vec_texture->push_back( y_bottom );
        //####//
        vec_vertex->push_back( inx + x_off );
        vec_vertex->push_back( iny + y_off - height);
        vec_vertex->push_back( 0 );
        ver_count++;

        //####//
        //####//
        //####//

        vec_texture->push_back( x_right );
        vec_texture->push_back( y_bottom );
        //####//
        vec_vertex->push_back( inx + x_off + width );
        vec_vertex->push_back( iny + y_off - height);
        vec_vertex->push_back( 0 );
        ver_count++;

        keep_inx = inx;
        inx+=(int)(mit->second.advance * inscale );

        } else {
            break;
        }

    }

        vec_texture->push_back( x_right );
        vec_texture->push_back( y_bottom );
        //####//
        vec_vertex->push_back( keep_inx + x_off + width );
        vec_vertex->push_back( iny + y_off - height);
        vec_vertex->push_back( 0 );
        ver_count++;

  } else { //origin bottom left


    for ( a=0; a < phrase_len; a++ ) {

        //ABCDEF+ FGGH + GHIJKL

        i_glyph = intext[a];
        //printf("Operating on glyph %d\n",i_glyph);

        //check to see if given value is even in the map.  if not, continue to next char
        mit = glyphs.find(i_glyph);
        if ( mit == glyphs.end() ) continue;

        if ( !inmax || (inx+(int)(mit->second.advance * inscale )-old_inx) <= inmax ) {

        x_left = mit->second.x_left;  x_right = mit->second.x_right;
        y_top = mit->second.y_top;    y_bottom = mit->second.y_bottom;
        height = mit->second.height;  width = mit->second.width;
        x_off = mit->second.x_offset; y_off = mit->second.y_offset;

        width  = (int)((float)width  * inscale);
        height = (int)((float)height * inscale);
        x_off  = (int)((float)x_off  * inscale);
        y_off  = (int)((float)y_off  * inscale);


        if ( a==0 && vec_vertex->size()!=0 ) {

            vec_texture->push_back(x_left);
            vec_texture->push_back(y_top);
            //####//
            vec_vertex->push_back( inx + x_off );
            vec_vertex->push_back( iny - y_off );
            vec_vertex->push_back( 0 );
            ver_count++;

            vec_texture->push_back(x_left);
            vec_texture->push_back(y_top);
            //####//
            vec_vertex->push_back( inx + x_off );
            vec_vertex->push_back( iny - y_off );
            vec_vertex->push_back( 0 );
            ver_count++;

            vec_texture->push_back( x_right );
            vec_texture->push_back( y_top );
            //####//
            vec_vertex->push_back( inx + x_off + width );
            vec_vertex->push_back( iny - y_off );
            vec_vertex->push_back( 0 );
            ver_count++;

        }

        vec_texture->push_back(x_left);
        vec_texture->push_back(y_top);
        //####//
        vec_vertex->push_back( inx + x_off );
        vec_vertex->push_back( iny - y_off );
        vec_vertex->push_back( 0 );
        ver_count++;

        vec_texture->push_back( x_right );
        vec_texture->push_back( y_top );
        //####//
        vec_vertex->push_back( inx + x_off + width );
        vec_vertex->push_back( iny - y_off );
        vec_vertex->push_back( 0 );
        ver_count++;

        vec_texture->push_back( x_left );
        vec_texture->push_back( y_bottom );
        //####//
        vec_vertex->push_back( inx + x_off );
        vec_vertex->push_back( iny - y_off + height);
        vec_vertex->push_back( 0 );
        ver_count++;

        //####//
        //####//
        //####//

        vec_texture->push_back( x_right );
        vec_texture->push_back( y_bottom );
        //####//
        vec_vertex->push_back( inx + x_off + width );
        vec_vertex->push_back( iny - y_off + height);
        vec_vertex->push_back( 0 );
        ver_count++;

        keep_inx = inx;
        inx+=(int)(mit->second.advance * inscale );

        } else {
            break;
        }

    }

    vec_texture->push_back( x_right );
    vec_texture->push_back( y_bottom );
    //####//
    vec_vertex->push_back( keep_inx + x_off + width );
    vec_vertex->push_back( iny - y_off + height);
    vec_vertex->push_back( 0 );
    ver_count++;


  } //end origin selection

  if ( vec_color != NULL ) {
      for ( int i=0; i < ver_count; i++ ) {
          vec_color->push_back(r);
          vec_color->push_back(g);
          vec_color->push_back(b);
          vec_color->push_back(a);
      }
  }

  return inx-old_inx;
}




int freetype_font::checkLength( const char *intext, float inscale ) {

    int moved_x = 0;
    int phrase_len = strlen(intext);

    for ( int a=0; a < phrase_len; a++ ) {
        moved_x+=glyphs[intext[a]].advance;
    }
    return (int)((float)moved_x * inscale);

}


int freetype_font::clipText( char *intext, char **returntext, char **remainingtext, float inscale, int inmaxwidth ) {

    int moved_x;
    unsigned int a;

    if ( returntext != NULL ) {
        *returntext = new char[strlen(intext)+1];
        strcpy(*returntext, intext);
    }

    if ( remainingtext != NULL ) (*remainingtext) = intext + sizeof(char)*strlen(intext);

    moved_x = 0;

    for ( a=0; a < strlen(intext); a++ ) {
        moved_x+=glyphs[intext[a]-32].advance;
        if ( (int)((float)moved_x * inscale) > inmaxwidth ) {
            moved_x -= glyphs[intext[a]-32].advance;
             if ( returntext != NULL ) (*returntext)[a] = '\0';
            if ( remainingtext != NULL ) (*remainingtext) = intext + sizeof(char)*a;
            break;
        }
    }

    return (int)((float)moved_x * inscale);

}


int freetype_font::clipTextMulti( char *intext, std::list<char*> *vec_text, float inscale, int inmaxwidth ) {

    char *remain_text, *temp_text, *temp_text2;
    static std::list<char*>::iterator it;

    for ( it = (*vec_text).begin(); it != (*vec_text).end(); it++ ) {
        delete (*it);
    }
    (*vec_text).clear();

    temp_text = new char[strlen(intext)+1];
    strcpy(temp_text, intext);


    clipText(temp_text, &temp_text2, &remain_text, inscale, inmaxwidth);
    (*vec_text).push_back( new char[strlen(temp_text2)+1] );
    strcpy( (*vec_text).back(), temp_text2);

    while ( strlen(remain_text) ) {
        strcpy(temp_text, remain_text);
        delete (temp_text2);
        clipText(temp_text, &temp_text2, &remain_text, inscale, inmaxwidth);

        (*vec_text).push_back( new char[strlen(temp_text2)+1] );
        strcpy( (*vec_text).back(), temp_text2);
    }

    delete(temp_text2);
    return 0;
}


int freetype_font::returnOffset( char *intext, int position, float inscale ) {

    int moved_x, a;
    moved_x = 0;

    for ( a=0; a < position; a++ ) {

        moved_x+=(int)((float)glyphs[intext[a]-32].advance*inscale);
        //moved_x+=glyphs[intext[a]-32].x_offset;

        //printf("advance: %d , xoffset : %d, bearing x: %d\n", glyphs[intext[a]-32].advance, glyphs[intext[a]-32].x_offset, glyphs[intext[a]-32].x_offset);
    }

    //printf("(%f)(%s) font returned offset : %d\n", inscale, intext, (int)((float)moved_x * inscale) );

    return (int)((float)moved_x);

}



void freetype_font::repositionText( std::vector<GLfloat>* invec, int inx_off, int iny_off ) {
static int a;
static int veclength;

    veclength=invec->size();

        for ( a=2; a < veclength; a+=5 ) {

            (*invec)[a]   += inx_off;
            (*invec)[a+1] += iny_off;
        }
}


void freetype_font::setCorner(int in) {
    if ( in == FONT_ORIGIN_LOWERLEFT )
        origin_topleft = false;
    else
        origin_topleft = true;
}

int freetype_font::getMaxHeight() { return max_height; }

inline int freetype_font::next_p2 (int a) { int rval=1; while(rval<a) rval<<=1; return rval; }

void freetype_font::setPointerList( font_list_pointers *in ) { my_list = in; }

void freetype_font::setColor( float *in ) { r=in[0]; g=in[1]; b=in[2]; a=in[3]; }
void freetype_font::setColor( float inr, float ing, float inb, float ina ) { r=inr; g=ing; b=inb; a=ina; }
void freetype_font::setColor( int inr, int ing, int inb, int ina ) { r=(float)inr/255.0; g=(float)ing/255.0; b=(float)inb/255.0; a=(float)ina/255.0; }

GLuint freetype_font::getTextureID() { return atlas_texture; }
