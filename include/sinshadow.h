#include <list>
#include <vector>
#include <sinprim.h>



typedef struct shadow_volume_s {
    //general shadow stuff
    void *drawing_func;
    sinlight *lights;
    int num_of_lights;

    //projection info
    GLfloat infinite_matrix[16];
    GLfloat infinite_matrix[16];

    //volume stuff
    std::list<edge> edges;
    std::list<edge>::iterator it;
    std::list<int> edges_index;
    std::list<int>::iterator iti;
    std::vector<GLfloat> triangles;
    edge temp_edges[3];
    vec3f temp_point;
    bool dupe;
    int front_off_1, front_off_2, back_off_1, back_off_2;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    void generate_edges(const vec3f &light_pos, const std::vector<GLfloat> &in_data, const float &infinity) {
            if ( in_data.size() < 9 ) return;
            edges.clear();
            edges_index.clear();
            triangles.clear();
            for ( unsigned int i=0; i < in_data.size(); i+=9 ) {

                temp_edges[0].p1.x = in_data[i+0];
                temp_edges[0].p1.y = in_data[i+1];
                temp_edges[0].p1.z = in_data[i+2];
                temp_edges[0].p2.x = in_data[i+3];
                temp_edges[0].p2.y = in_data[i+4];
                temp_edges[0].p2.z = in_data[i+5];

                temp_edges[1].p2.x = in_data[i+6];
                temp_edges[1].p2.y = in_data[i+7];
                temp_edges[1].p2.z = in_data[i+8];

                if ( ((temp_edges[1].p2 - temp_edges[0].p2).cross(temp_edges[0].p2 - temp_edges[0].p1))*(temp_edges[0].p1 - light_pos) > 0 )
                    continue; //not facing

                temp_edges[1].p1.x = in_data[i+3];
                temp_edges[1].p1.y = in_data[i+4];
                temp_edges[1].p1.z = in_data[i+5];

                temp_edges[2].p1.x = in_data[i+6];
                temp_edges[2].p1.y = in_data[i+7];
                temp_edges[2].p1.z = in_data[i+8];
                temp_edges[2].p2.x = in_data[i+0];
                temp_edges[2].p2.y = in_data[i+1];
                temp_edges[2].p2.z = in_data[i+2];

                //add triangle to the triangles list for front shadow cap
                triangles.push_back(in_data[i+0]); triangles.push_back(in_data[i+1]); triangles.push_back(in_data[i+2]);
                triangles.push_back(in_data[i+3]); triangles.push_back(in_data[i+4]); triangles.push_back(in_data[i+5]);
                triangles.push_back(in_data[i+6]); triangles.push_back(in_data[i+7]); triangles.push_back(in_data[i+8]);

                //add triangles to list for back shadow cap
                temp_point = ((temp_edges[1].p2 - light_pos).norm()) * infinity + temp_edges[1].p2;
                triangles.push_back(temp_point.x); triangles.push_back(temp_point.y); triangles.push_back(temp_point.z);
                temp_point = ((temp_edges[0].p2 - light_pos).norm()) * infinity + temp_edges[0].p2;
                triangles.push_back(temp_point.x); triangles.push_back(temp_point.y); triangles.push_back(temp_point.z);
                temp_point = ((temp_edges[0].p1 - light_pos).norm()) * infinity + temp_edges[0].p1;
                triangles.push_back(temp_point.x); triangles.push_back(temp_point.y); triangles.push_back(temp_point.z);

                //add edge to set
                for ( int e=0; e<3; ++e ) {
                    dupe = false;
                    for (it=edges.begin(),iti=edges_index.begin(); it != edges.end(); ++it,++iti) {
                        if ( (*it) == temp_edges[e] ) { dupe = true; edges.erase(it); edges_index.erase(iti); break; }
                    }
                    if ( !dupe ) {
                        edges.push_back(temp_edges[e]);
                        edges_index.push_back(((triangles.size()-18)<<2) | e);
                    }
                }

            }

            //now that dupes have been removed, create the sihlouette triangles
            for (iti=edges_index.begin(); iti != edges_index.end(); ++iti) {
                front_off_1 = front_off_2 = back_off_1 = back_off_2 = (*iti)>>2;
                switch ( (*iti)&3 ) {
                    case 0: front_off_1 += 0; front_off_2 += 3; back_off_1 += 15; back_off_2 += 12; break;
                    case 1: front_off_1 += 3; front_off_2 += 6; back_off_1 += 12; back_off_2 +=  9; break;
                    case 2: front_off_1 += 6; front_off_2 += 0; back_off_1 +=  9; back_off_2 += 15; break;
                }
                triangles.push_back( triangles[front_off_1] ); triangles.push_back( triangles[front_off_1+1] ); triangles.push_back( triangles[front_off_1+2] );
                triangles.push_back( triangles[back_off_1] ); triangles.push_back( triangles[back_off_1+1] ); triangles.push_back( triangles[back_off_1+2] );
                triangles.push_back( triangles[front_off_2] ); triangles.push_back( triangles[front_off_2+1] ); triangles.push_back( triangles[front_off_2+2] );
                triangles.push_back( triangles[front_off_2] ); triangles.push_back( triangles[front_off_2+1] ); triangles.push_back( triangles[front_off_2+2] );
                triangles.push_back( triangles[back_off_1] ); triangles.push_back( triangles[back_off_1+1] ); triangles.push_back( triangles[back_off_1+2] );
                triangles.push_back( triangles[back_off_2] ); triangles.push_back( triangles[back_off_2+1] ); triangles.push_back( triangles[back_off_2+2] );
            }

        }


    /////////////////////
    void shadow_vodou() {
            bool unsafe_clamping = true;

            for ( int i=0; i<num_of_lights; ++i ) {

                for ( int ii=0; ii<num_of_lights; ++ii )
                    glDisable(GL_LIGHT0+ii);

                glClear( GL_STENCIL_BUFFER_BIT );

                if ( !GLEE_NV_depth_clamp ) {
                    //without depth clamping we have to render the scene twice
                    if ( !unsafe_clamping ) {
                        glMatrixMode(GL_PROJECTION);
                        glPushMatrix();
                        glLoadIdentity();
                        perspectiveGL(45.0f,((GLfloat)x_rez)/((GLfloat)y_rez),0.1f,-1.0);
                        glMatrixMode(GL_MODELVIEW);

                        glColorMask(0,0,0,0);
                        drawing_func();
                        glDepthMask(0);
                    } else {
                        if ( i == 0 ) drawing_func();
                        glColorMask(0,0,0,0);
                        glDepthMask(0);
                    }
                } else {
                    if ( i == 0 ) drawing_func();
                    glColorMask(0,0,0,0);
                    glDepthMask(0);
                    glEnable(GL_DEPTH_CLAMP_NV);
                }


                glEnable(GL_STENCIL_TEST);
                glEnable(GL_STENCIL_TEST_TWO_SIDE_EXT);
                glDisable(GL_CULL_FACE);

                glActiveStencilFaceEXT(GL_BACK); //glCullFace(GL_FRONT);
                glStencilOp(GL_KEEP, GL_INCR_WRAP, GL_KEEP);
                glStencilFunc(GL_ALWAYS, 0, ~0); //glStencilMask(~0);
                glActiveStencilFaceEXT(GL_FRONT); //glCullFace(GL_BACK);
                glStencilOp(GL_KEEP, GL_DECR_WRAP, GL_KEEP);
                glStencilFunc(GL_ALWAYS, 0, ~0); //glStencilMask(~0);

                //glEnable( GL_POLYGON_OFFSET_FILL );
                //glPolygonOffset( 1.0f, 1.0f );

                glEnableClientState(GL_VERTEX_ARRAY);
                    glVertexPointer(3, GL_FLOAT, 0, &my_volume[i].triangles[0] );
                    glDrawArrays(GL_TRIANGLES, 0, my_volume[i].triangles.size()/3 );
                glDisableClientState(GL_VERTEX_ARRAY);

                //glDisable( GL_POLYGON_OFFSET_FILL );

                glEnable(GL_CULL_FACE);
                glDisable(GL_STENCIL_TEST_TWO_SIDE_EXT);
                glColorMask(1,1,1,1);

                if ( !GLEE_NV_depth_clamp ) {
                  if ( !unsafe_clamping ) {
                    glMatrixMode(GL_PROJECTION);
                    glPopMatrix();
                    glMatrixMode(GL_MODELVIEW);

                    //render with only ambients first, including depth
                    if ( i != 0 ) glColorMask(0,0,0,0);
                    glDisable(GL_STENCIL_TEST);
                    glDepthMask(1);
                    glClear(GL_DEPTH_BUFFER_BIT);
                    drawing_func();
                    glEnable(GL_STENCIL_TEST);
                    glColorMask(1,1,1,1);
                  }
                } else {
                    glDisable(GL_DEPTH_CLAMP_NV);
                }

                glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
                glStencilFunc(GL_EQUAL, 0, ~0);

                glEnable(GL_LIGHT0+i);
                glDepthFunc(GL_LEQUAL);
                glEnable(GL_BLEND);
                glBlendFunc(GL_ONE, GL_ONE);
                glDepthMask(0);
                drawing_func();

                glDisable(GL_STENCIL_TEST);
                glDepthMask(1);
                glDepthFunc(GL_LESS);
                glDisable(GL_BLEND);
            }
        }

} shadow_volume;
