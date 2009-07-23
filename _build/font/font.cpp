
#include "sinfont.h"


glyph_info::glyph_info( char a, char b, float c, float d, float e, float f, short int g, short int h, short int i, char j ) {
x_offset = a; y_offset = b; x_left = c; x_right = d; y_top = e; y_bottom = f; advance = g; height = h; width = i; character = j;  }



int freetype_font::init( const char *infile, int insize, bool origin ) {

    int error;
    int x_width = 0;
    int y_height = 0;
    int x_max_width = 0;
    int y_max_height = 0;
    int y_max_top = 0;
    int tex_width=0;
    int tex_height=0;
    int max_tot_height=0;
    int count=0;
    int pen_x=0;
    int pen_y=2;
    int pen_offset;

    origin_topleft = origin;

    printf("* INIT: Trying to init FreeType... ");

    my_matrix = new glyph_matrix[95];

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

    printf("past main inits... ");

    //find the width of texture
    for ( int char_offset = 0; char_offset < 94; char_offset++ ) {
        error = FT_Load_Char( face, 32+char_offset, FT_LOAD_RENDER );
        if ( error ) { printf("ERROR in FreeType - FT_Load_Char\n"); }

        bitmap = (FT_Bitmap*)&slot->bitmap;
        x_width += ((FT_Bitmap*)&slot->bitmap)->width;
        count++;
        if ( count == 10 ) {
            //printf("%d\n",x_width);
            if ( x_width > x_max_width ) { x_max_width = x_width; }
            count = 0;
            x_width=0;
            }

        y_height = ((FT_Bitmap*)&slot->bitmap)->rows;
        if ( y_height > y_max_height ) { y_max_height = y_height; }

        if ( slot->bitmap_top > y_max_top ) { y_max_top = slot->bitmap_top; }

        if ( -1*((bitmap->rows - y_max_top)+(slot->bitmap_top - bitmap->rows))+y_height > max_tot_height ) {
            max_tot_height = -1*((bitmap->rows - y_max_top)+(slot->bitmap_top - bitmap->rows))+y_height; }

    }

    //printf("\nmax width: %d (%d) ",x_max_width,next_p2(x_max_width));
    //printf("max height: %d (%d) ",y_max_height,next_p2(y_max_height*10));
    //printf("max top: %d\n",y_max_top);

    tex_width = next_p2(x_max_width+10); //BUG: +10 cause i was getting segfaults here
    tex_height = next_p2(y_max_height*8);

    atlas_data = new GLubyte[ 2 * tex_width * tex_height ];
    for (int i=0; i < tex_height*tex_width; i++) {
        //default alpha value
        atlas_data[i] = 0; }

    pen_x=1; pen_y=1;
    for ( int char_offset = 0; char_offset <= 94; char_offset++ ) {
        error = FT_Load_Char( face, 32+char_offset, FT_LOAD_RENDER );
        if ( error ) { printf("ERROR in FreeType - FT_Load_Char\n"); }
        bitmap = &slot->bitmap;

            if ( (pen_x + bitmap->width) > (tex_width-2) ) {
                pen_y += y_max_height+1; pen_x = 1;
            }

            //printf("%d %d %d %d %d | ",pen_x,pen_y,tex_width, bitmap->width, (slot->advance.x >> 6));
            //printf("%d ",bitmap->num_grays);

            my_matrix[char_offset].bools = new bool[bitmap->width * bitmap->rows];

            //printf("%d %d = %d\n",bitmap->rows, pen_y, (bitmap->rows-1-pen_y)*tex_width);
            pen_offset = pen_x + ((tex_height-y_max_height-pen_y) * tex_width);
            for(int j=0; j <bitmap->rows;j++) {
                for(int i=0; i < bitmap->width; i++){
                    atlas_data[(pen_offset+i+(bitmap->rows-1-j)*tex_width)] = bitmap->buffer[i + bitmap->width*j];
                    //atlas_data[1+2*(pen_offset+i+(bitmap->rows-1-j)*tex_width)] = bitmap->buffer[i + bitmap->width*j];
                    my_matrix[char_offset].bools[i+j*bitmap->width] = ( bitmap->buffer[i + bitmap->width*j] >= 64 ? true : false );
                    //printf("%d\n", bitmap->buffer[i + bitmap->width*j] );
                }
            }


            my_matrix[char_offset].w = bitmap->width;
            my_matrix[char_offset].h = bitmap->rows;
            my_matrix[char_offset].xoffset = slot->bitmap_left;
            my_matrix[char_offset].yoffset = (bitmap->rows - y_max_top)+(slot->bitmap_top - bitmap->rows);
            my_matrix[char_offset].advance = slot->advance.x >> 6;

            glyphs.push_back( glyph_info( slot->bitmap_left,               // x_offset
                                          (bitmap->rows - y_max_top)+(slot->bitmap_top - bitmap->rows), // y_offset
                                          (float)(pen_x)/(float)tex_width, //left
                                          (float)(pen_x+bitmap->width)/(float)tex_width, //right
                                          1.0-(float)(pen_y + (y_max_height-bitmap->rows))/(float)tex_height, //top
                                          1.0-(float)(pen_y + y_max_height )/(float)tex_height, //bottom
                                          slot->advance.x >> 6,            // advance
                                          bitmap->rows,                    // height
                                          bitmap->width,                   // width
                                          32+char_offset                   //ascii char
                                          ) );

            pen_x += -slot->bitmap_left+2+(slot->advance.x >> 6);

    //printf("yar: %+3d %+3d %+3d\n",(bitmap->rows - y_max_height), (slot->bitmap_top - bitmap->rows), (bitmap->rows - y_max_height)+(slot->bitmap_top - bitmap->rows));

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

    max_height = max_tot_height;

    printf("done.\n");

/*
        error = FT_Load_Char( face, '5', FT_LOAD_RENDER );
        if ( error ) { printf("ERROR in FreeType - FT_Load_Char\n"); }
        bitmap = &slot->bitmap;

        printf("\na:\nwidth: %d, height: %d, pitch: %d\n",bitmap->width, bitmap->rows, bitmap->pitch);

            for(int j=0; j <bitmap->rows;j++) {
                for(int i=0; i < bitmap->width; i++){
                    //for (k=0; k < 8; k++) {
                    //if ( bitmap->buffer[
                    if ( 1 || (bitmap->buffer[bitmap->pitch*j+i]) > 128)
                        //printf("*");
                        printf("%3d ",bitmap->buffer[bitmap->pitch*j+i]);
                    else if ( (bitmap->buffer[bitmap->pitch*j+i]) > 64 )
                        printf("+");
                    else if ( (bitmap->buffer[bitmap->pitch*j+i]) > 1 )
                        printf("'");
                    else
                        printf(" ");
                    //}

                    //atlas_data[16*j+i] = bitmap->buffer[bitmap->pitch*j+i];


                }
                printf("\n");
            }

        printf("pitch: %d, left: %d, top: %d, rows: %d \n", bitmap->pitch, slot->bitmap_left,  slot->bitmap_top, bitmap->rows);
*/

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
  } else {
      vec_vertex = invec;
      vec_texture = invec;
      vec_color = NULL;
  }

  if ( origin_topleft ) {

    for ( a=0; a < phrase_len; a++ ) {

        //ABCDEF+ FGGH + GHIJKL

        i_glyph = intext[a]-32;
        //printf("Operating on glyph %d\n",i_glyph);

        if ( !inmax || (inx+(int)(glyphs[i_glyph].advance * inscale )-old_inx) <= inmax ) {

        x_left = glyphs[i_glyph].x_left;  x_right = glyphs[i_glyph].x_right;
        y_top = glyphs[i_glyph].y_top;    y_bottom = glyphs[i_glyph].y_bottom;
        height = glyphs[i_glyph].height;  width = glyphs[i_glyph].width;
        x_off = glyphs[i_glyph].x_offset; y_off = glyphs[i_glyph].y_offset;

        width  = (int)((float)width  * inscale);
        height = (int)((float)height * inscale);
        x_off  = (int)((float)x_off  * inscale);
        y_off  = (int)((float)y_off  * inscale);

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
        inx+=(int)(glyphs[i_glyph].advance * inscale );

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

        i_glyph = intext[a]-32;
        //printf("Operating on glyph %d\n",i_glyph);

        if ( !inmax || (inx+(int)(glyphs[i_glyph].advance * inscale )-old_inx) <= inmax ) {

        x_left = glyphs[i_glyph].x_left;  x_right = glyphs[i_glyph].x_right;
        y_top = glyphs[i_glyph].y_top;    y_bottom = glyphs[i_glyph].y_bottom;
        height = glyphs[i_glyph].height;  width = glyphs[i_glyph].width;
        x_off = glyphs[i_glyph].x_offset; y_off = glyphs[i_glyph].y_offset;

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
        inx+=(int)(glyphs[i_glyph].advance * inscale );

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

    int moved_x, a, phrase_len;

    moved_x = 0;
    phrase_len = strlen(intext);
    for ( a=0; a < phrase_len; a++ ) {
        moved_x+=glyphs[intext[a]-32].advance;
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








glyph_matrix* freetype_font::returnCharMatrix( char inchar ) {

    return &my_matrix[inchar-32];


}

