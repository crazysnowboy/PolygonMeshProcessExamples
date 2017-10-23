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
//  CLASS FairingViewer
//
//=============================================================================


#ifndef FAIRING_VIEWER_HH
#define FAIRING_VIEWER_HH


//== INCLUDES =================================================================


#include "CurvatureViewer.h"
#include <map>


//== CLASS DEFINITION =========================================================

	      

class FairingViewer : public CurvatureViewer
{
public:
   
  /// default constructor
  FairingViewer(const char* _title, int _width, int _height);

  /// iterative Laplacian smoothing
  void fair();



protected:


  virtual void keyboard(int key, int x, int y);


  // easier access to new vertex positions
  int& idx(Mesh::VertexHandle _vh) { return mesh_.property(vidx_, _vh); }



  // helper data structure
  struct Triple
  { 
    Triple() {}
    Triple(VertexHandle _vh, double _weight, unsigned int _ld)
      : vh(_vh), weight(_weight), degree(_ld) {}
    VertexHandle  vh;
    double        weight;
    unsigned int  degree;
  };


  // build one row of a bi-Laplacian matrix
  void setup_matrix_row(Mesh::VertexHandle                    _vh,
			unsigned int                          _laplace_degree,
			double                                _weight,
			std::map<Mesh::VertexHandle,double>&  _row);


protected:

  OpenMesh::VPropHandleT<int>  vidx_;
};


//=============================================================================
#endif // FAIRING_VIEWER_HH defined
//=============================================================================

