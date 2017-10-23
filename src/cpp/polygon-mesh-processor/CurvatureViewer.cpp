//=============================================================================
//
//   Example code for the full-day course
//
//   M. Botsch, M. Pauly, L. Kobbelt, P. Alliez, B. Levy,
//   "Geometric Modeling Based on Polygonal Meshes"
//   held at SIGGRAPH 2007, San Diego, and Eurographics 2008, Crete.
//
//   Copyright (C) 2007 by  Computer Graphics Laboratory, ETH Zurich,
//                      and Computer Graphics Group,      RWTH Aachen
//
//
//-----------------------------------------------------------------------------
//
//                                License
//
//   This program is free software; you can redistribute it and/or
//   modify it under the terms of the GNU General Public License
//   as published by the Free Software Foundation; either version 2
//   of the License, or (at your option) any later version.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 51 Franklin Street, Fifth Floor,
//   Boston, MA  02110-1301, USA.
//
//=============================================================================
//=============================================================================
//
//  CLASS CurvatureViewer - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

#include "CurvatureViewer.h"
#include <vector>
#include <float.h>



//== IMPLEMENTATION ==========================================================


CurvatureViewer::
CurvatureViewer(const char* _title, int _width, int _height)
    : MeshViewer(_title, _width, _height)
{
    mesh_.request_vertex_colors();

    mesh_.add_property(vcurvature_);
    mesh_.add_property(vweight_);
    mesh_.add_property(eweight_);

    add_draw_mode("Curvature");
    add_draw_mode("Reflection Lines");

    init();
}


//-----------------------------------------------------------------------------


CurvatureViewer::
~CurvatureViewer()
{
    if (glIsTexture(textureID_))
    {
        glDeleteTextures( 1, &textureID_);
    }
}

//-----------------------------------------------------------------------------


void
CurvatureViewer::
init()
{
    // base class first
    MeshViewer::init();


    // generate checkerboard-like image
    GLubyte tex[256*256*3], *tp=tex;
    for (int x=0; x<256; ++x)
        for (int y=0; y<256; ++y)
            if (((x+2)/4 % 10) == 0 || ((y+2)/4 % 10) == 0)
            {
                *(tp++) = 0;
                *(tp++) = 0;
                *(tp++) = 0;
            }
            else
            {
                *(tp++) = 255;
                *(tp++) = 255;
                *(tp++) = 255;
            }


    // generate texture
    if (!glIsTexture(textureID_))
    {
        glGenTextures(1, &textureID_);
    }
    glBindTexture(GL_TEXTURE_2D, textureID_);


    // copy texture to GL
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, 256, 256,
                 0, GL_RGB, GL_UNSIGNED_BYTE, tex);
}



//-----------------------------------------------------------------------------


bool
CurvatureViewer::
open_mesh(const char* _filename)
{
    // load mesh
    if (MeshViewer::open_mesh(_filename))
    {
        // compute curvature stuff
        calc_weights();
        calc_curvature();
        color_coding();

        glutPostRedisplay();
        return true;
    }
    return false;
}


//-----------------------------------------------------------------------------


void
CurvatureViewer::
calc_weights()
{
    Mesh::VertexIter        v_it, v_end(mesh_.vertices_end());
    Mesh::EdgeIter          e_it, e_end(mesh_.edges_end());
    Mesh::VertexFaceIter    vf_it;
    Mesh::FaceVertexIter    fv_it;
    Mesh::HalfedgeHandle    h0, h1, h2;
    Mesh::VertexHandle      v0, v1;
    Mesh::Point             p0, p1, p2, d0, d1;
    Mesh::Scalar            w, area, b(0.99);



    for (e_it=mesh_.edges_begin(); e_it!=e_end; ++e_it)
    {
        w  = 0.0;

        h0 = mesh_.halfedge_handle(e_it.handle(), 0);
        v0 = mesh_.to_vertex_handle(h0);
        p0 = mesh_.point(v0);

        h1 = mesh_.halfedge_handle(e_it.handle(), 1);
        v1 = mesh_.to_vertex_handle(h1);
        p1 = mesh_.point(v1);

        if (!mesh_.is_boundary(h0))
        {
            h2 = mesh_.next_halfedge_handle(h0);
            p2 = mesh_.point(mesh_.to_vertex_handle(h2));
            d0 = (p0 - p2).normalize();
            d1 = (p1 - p2).normalize();
            w += 1.0 / tan(acos(std::max(-b, std::min(b, (d0|d1)))));
        }

        if (!mesh_.is_boundary(h1))
        {
            h2 = mesh_.next_halfedge_handle(h1);
            p2 = mesh_.point(mesh_.to_vertex_handle(h2));
            d0 = (p0 - p2).normalize();
            d1 = (p1 - p2).normalize();
            w += 1.0 / tan(acos(std::max(-b, std::min(b, (d0|d1)))));
        }

        // force weights to be non-negative for higher robustness
        w = std::max(w, 0.0f);

        weight(e_it) = w;
    }


    for (v_it=mesh_.vertices_begin(); v_it!=v_end; ++v_it)
    {
        area = 0.0;

        for (vf_it=mesh_.vf_iter(v_it); vf_it; ++vf_it)
        {
            fv_it = mesh_.fv_iter(vf_it);

            const Mesh::Point& P = mesh_.point(fv_it);
            ++fv_it;
            const Mesh::Point& Q = mesh_.point(fv_it);
            ++fv_it;
            const Mesh::Point& R = mesh_.point(fv_it);

            area += ((Q-P)%(R-P)).norm() * 0.5f * 0.3333f;
        }

        weight(v_it) = (fabs(area)>FLT_MIN ? 1.0 / (2.0 * area) : 0.0);
    }
}


//-----------------------------------------------------------------------------


void CurvatureViewer::calc_curvature()
{
    Mesh::VertexIter        v_it, v_end(mesh_.vertices_end());
    Mesh::HalfedgeHandle    h;
    Mesh::EdgeHandle        e;
    Mesh::VertexVertexIter  vv_it;
    Mesh::Point             laplace(0.0, 0.0, 0.0);


    for (v_it=mesh_.vertices_begin(); v_it!=v_end; ++v_it)
    {
        curvature(v_it) = 0.0;
        laplace = Mesh::Point(0,0,0);

        if (!mesh_.is_boundary(v_it.handle()))
        {
            for (vv_it=mesh_.vv_iter(v_it); vv_it; ++vv_it)
            {
                h = vv_it.current_halfedge_handle();
                e = mesh_.edge_handle(h);

                laplace += weight(e) * (mesh_.point(vv_it) - mesh_.point(v_it));
            }
            laplace *= weight(v_it);

            curvature(v_it) = laplace.norm();
        }
    }
}


//-----------------------------------------------------------------------------


void CurvatureViewer::color_coding()
{

    Mesh::VertexIter  v_it, v_end(mesh_.vertices_end());
    Mesh::Scalar      curv, min_curv(FLT_MAX), max_curv(-FLT_MAX);
    Mesh::Color       col;


    // put all curvature values into one array
    std::vector<Mesh::Scalar> curv_values;
    curv_values.reserve(mesh_.n_vertices());
    for (v_it=mesh_.vertices_begin(); v_it!=v_end; ++v_it)
    {
        curv_values.push_back(curvature(v_it));
    }


    // discard upper and lower 5%
    unsigned int n = curv_values.size()-1;
    unsigned int i = n / 20;
    std::sort(curv_values.begin(), curv_values.end());
    min_curv = curv_values[i];
    max_curv = curv_values[n-1-i];


    // define uniform color intervalls [v0,v1,v2,v3,v4]
    Mesh::Scalar v0, v1, v2, v3, v4;
    v0 = min_curv + 0.0/4.0 * (max_curv - min_curv);
    v1 = min_curv + 1.0/4.0 * (max_curv - min_curv);
    v2 = min_curv + 2.0/4.0 * (max_curv - min_curv);
    v3 = min_curv + 3.0/4.0 * (max_curv - min_curv);
    v4 = min_curv + 4.0/4.0 * (max_curv - min_curv);



    // map curvatures to colors
    for (v_it=mesh_.vertices_begin(); v_it!=v_end; ++v_it)
    {
        curv = curvature(v_it);
        col = Mesh::Color(255,255,255);

        unsigned char u;

        if (curv < v0)
        {
            col = Mesh::Color(0, 0, 255);
        }
        else if (curv > v4)
        {
            col = Mesh::Color(255, 0, 0);
        }

        else if (curv <= v2)
        {
            if (curv <= v1) // [v0, v1]
            {
                u = (unsigned char) (255.0 * (curv - v0) / (v1 - v0));
                col = Mesh::Color(0, u, 255);
            }
            else // ]v1, v2]
            {
                u = (unsigned char) (255.0 * (curv - v1) / (v2 - v1));
                col = Mesh::Color(0, 255, 255-u);
            }
        }
        else
        {
            if (curv <= v3) // ]v2, v3]
            {
                u = (unsigned char) (255.0 * (curv - v2) / (v3 - v2));
                col = Mesh::Color(u, 255, 0);
            }
            else // ]v3, v4]
            {
                u = (unsigned char) (255.0 * (curv - v3) / (v4 - v3));
                col = Mesh::Color(255, 255-u, 0);
            }
        }

        mesh_.set_color(v_it, col);
    }
}


//-----------------------------------------------------------------------------


void
CurvatureViewer::
draw(const std::string& _draw_mode)
{
    if (_draw_mode == "Curvature")
    {
        glDisable(GL_LIGHTING);
        glShadeModel(GL_SMOOTH);

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        GL::glVertexPointer(mesh_.points());
        GL::glNormalPointer(mesh_.vertex_normals());
        GL::glColorPointer(mesh_.vertex_colors());

        glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, &indices_[0]);

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
    }


    else if (_draw_mode == "Reflection Lines")
    {
        glTexGeni( GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP );
        glTexGeni( GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP );
        glEnable( GL_TEXTURE_GEN_S );
        glEnable( GL_TEXTURE_GEN_T );
        glEnable( GL_TEXTURE_2D );
        glEnable(GL_LIGHTING);
        glShadeModel(GL_SMOOTH);

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        GL::glVertexPointer(mesh_.points());
        GL::glNormalPointer(mesh_.vertex_normals());

        glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, &indices_[0]);

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);

        glDisable( GL_TEXTURE_GEN_S );
        glDisable( GL_TEXTURE_GEN_T );
        glDisable( GL_TEXTURE_2D );
    }



    else
    {
        MeshViewer::draw(_draw_mode);
    }
}


//=============================================================================
