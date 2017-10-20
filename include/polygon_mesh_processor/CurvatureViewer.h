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
//  CLASS CurvatureViewer
//
//=============================================================================


#ifndef CURVVIEWERWIDGET_HH
#define CURVVIEWERWIDGET_HH


//== INCLUDES =================================================================


#include "MeshViewer.hh"



//== CLASS DEFINITION =========================================================



class CurvatureViewer : public MeshViewer
{
public:

    /// default constructor
    CurvatureViewer(const char* _title, int _width, int _height);

    // destructor
    ~CurvatureViewer();

    /// open mesh
    virtual bool open_mesh(const char* _filename);



protected:

    virtual void init();
    virtual void draw(const std::string& _draw_mode);


    /// calculate vertex and edge weights
    void calc_weights();

    /// calculate curvature per vertex
    void calc_curvature();

    /// set vertex color from vertex curvature
    void color_coding();


    // easier access to vertex weights
    Mesh::Scalar& weight(Mesh::VertexHandle _vh)
    {
        return mesh_.property(vweight_, _vh);
    }

    // easier access to vertex curvature
    Mesh::Scalar& curvature(Mesh::VertexHandle _vh)
    {
        return mesh_.property(vcurvature_, _vh);
    }

    // easier access to edge weights
    Mesh::Scalar& weight(Mesh::EdgeHandle _eh)
    {
        return mesh_.property(eweight_, _eh);
    }



private:

    OpenMesh::VPropHandleT<Mesh::Scalar>  vweight_, vcurvature_;
    OpenMesh::EPropHandleT<Mesh::Scalar>  eweight_;

    GLuint  textureID_;
};


//=============================================================================
#endif // CURVVIEWERWIDGET_HH defined
//=============================================================================

