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
//  CLASS SmoothingViewer - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

#include "SmoothingViewer.h"



//== IMPLEMENTATION ==========================================================


SmoothingViewer::
SmoothingViewer(const char* _title, int _width, int _height)
    : CurvatureViewer(_title, _width, _height)
{
    mesh_.add_property(vpos_);
}


//-----------------------------------------------------------------------------


void SmoothingViewer::keyboard(int key, int x, int y)
{
    switch (key)
    {
    case ' ':
    {
        std::cout << "10 smoothing iterations: " << std::flush;
        smooth(10);
        calc_weights();
        calc_curvature();
        color_coding();
        glutPostRedisplay();
        std::cout << "done\n";
        break;
    }

    default:
    {
        CurvatureViewer::keyboard(key, x, y);
        break;
    }
    }
}


//-----------------------------------------------------------------------------


void SmoothingViewer::smooth(unsigned int _iters)
{
    Mesh::VertexIter        v_it, v_end(mesh_.vertices_end());
    Mesh::HalfedgeHandle    h;
    Mesh::EdgeHandle        e;
    Mesh::VertexVertexIter  vv_it;
    Mesh::Point             laplace(0.0, 0.0, 0.0);
    Mesh::Scalar            w, ww;


    for (unsigned int iter=0; iter<_iters; ++iter)
    {

        // compute new vertex positions by Laplacian smoothing
        for (v_it=mesh_.vertices_begin(); v_it!=v_end; ++v_it)
        {
            laplace = Mesh::Point(0,0,0);
            ww = 0.0;

            if (!mesh_.is_boundary(v_it))
            {
                for (vv_it=mesh_.vv_iter(v_it); vv_it; ++vv_it)
                {
                    h = vv_it.current_halfedge_handle();
                    e = mesh_.edge_handle(h);
                    w = weight(e);
                    ww += w;

                    laplace += w * (mesh_.point(vv_it) - mesh_.point(v_it));
                }

                laplace /= ww;   // normalize by sum of weights
                laplace *= 0.5;  // damping
            }

            new_pos(v_it) = mesh_.point(v_it) + laplace;
        }



        // update vertex positions
        for (v_it=mesh_.vertices_begin(); v_it!=v_end; ++v_it)
        {
            mesh_.set_point(v_it, new_pos(v_it));
        }
    }


    // update face and vertex normals
    mesh_.update_normals();
}


//=============================================================================
