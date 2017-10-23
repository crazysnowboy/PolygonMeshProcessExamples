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
//  CLASS FairingViewer - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

#include "FairingViewer.hh"
#include "TaucsSolver.hh"



//== IMPLEMENTATION ========================================================== 


FairingViewer::FairingViewer(const char* _title, int _width, int _height)
  : CurvatureViewer(_title, _width, _height)
{ 
  mesh_.add_property(vidx_);
  mesh_.request_vertex_status();
}


//-----------------------------------------------------------------------------


void
FairingViewer::
keyboard(int key, int x, int y)
{
  switch (key)
  {
    case ' ':
    {
      std::cout << "Bi-Laplacian fairing " << std::flush;
      fair();
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


void 
FairingViewer::
fair()
{
  Mesh::VertexIter  v_it, v_end(mesh_.vertices_end());
  Mesh::VVIter      vv_it;



  // we need a mesh w/ boundary for proper boundary constraints
  bool has_boundary=false;

  for (v_it=mesh_.vertices_begin(); v_it!=v_end; ++v_it)
  {
    if (mesh_.is_boundary(v_it))
    {
      has_boundary = true;
      break;
    }
  }

  if (!has_boundary)
  {
    std::cerr << "Need mesh with boundary\n";
    exit(1);
  }

      


  // lock two rings of boundary vertices
  for (v_it=mesh_.vertices_begin(); v_it!=v_end; ++v_it)
  {
    if (mesh_.is_boundary(v_it))
    {
      mesh_.status(v_it).set_locked(true);
      for (vv_it=mesh_.vv_iter(v_it); vv_it; ++vv_it)
	mesh_.status(vv_it).set_locked(true);
    }
  }



  // setup free vertices
  unsigned int  i;
  std::vector<Mesh::VertexHandle>   free_vertices;

  for (i=0, v_it=mesh_.vertices_begin(); v_it!=v_end; ++v_it)
  {
    if (!mesh_.status(v_it).locked())
    {
      idx(v_it) = i++;
      free_vertices.push_back(v_it);
    }
  }



  TaucsSolver          solver;
  unsigned int         N = free_vertices.size();
  std::vector<Vec3d>   b(N), x(N);

  std::map<VertexHandle,double>  row;
  std::map<VertexHandle,double>::const_iterator r_it, r_end;



  // setup matrix and rhs
  for (i=0; i<N; ++i)
  {
    row.clear();
    setup_matrix_row(free_vertices[i], 2, 1.0, row);

    solver.begin_row();

    b[i] = Vec3d(0,0,0);

    for (r_it=row.begin(), r_end=row.end(); r_it!=r_end; ++r_it)
      if (mesh_.status(r_it->first).locked())
	b[i] -= r_it->second * (Vec3d) mesh_.point(r_it->first);
      else
	solver.add_value(idx(r_it->first), r_it->second);

    solver.end_row();
  }


  // factorize & solve for x,y,z
  if (!solver.factorize() || !solver.vec_solve(b, x))
  {
    std::cerr << "Solving failed\n";
    exit(1);
  }


  // copy solution
  for (i=0; i<N; ++i)
    mesh_.set_point(free_vertices[i], (Mesh::Point)x[i]);



  // update face and vertex normals
  mesh_.update_normals();
}


//-----------------------------------------------------------------------------


void
FairingViewer::
setup_matrix_row(VertexHandle                    _vh,
		 unsigned int                    _laplace_degree,
		 double                          _weight,
		 std::map<VertexHandle,double>&  _row)
{
  std::vector<Triple>     todo; 
  Mesh::VertexVertexIter  vv_it;
  Mesh::VertexHandle      vh;
  Mesh::EdgeHandle        eh;
  double                  w, ww;
  unsigned int            d;
  Triple                  t(_vh, _weight, _laplace_degree);


  // init
  todo.reserve(50);
  todo.push_back(t);


  while (!todo.empty())
  {
    t  = todo.back(); todo.pop_back();
    vh = t.vh;
    d  = t.degree;

    if (d == 0)
    {
      _row[vh] += t.weight;
    }

    else
    {
      ww = 0.0;

      for (vv_it=mesh_.vv_iter(vh); vv_it; ++vv_it)
      {
	eh  = mesh_.edge_handle(vv_it.current_halfedge_handle());
	w   = weight(eh);

	if (d < _laplace_degree) 
	  w  *= weight(vh);

	w  *= t.weight;
	ww -= w;

	todo.push_back(Triple(vv_it, w, d-1));
      }

      todo.push_back(Triple(vh, ww, d-1));
    }
  }
}


//=============================================================================
