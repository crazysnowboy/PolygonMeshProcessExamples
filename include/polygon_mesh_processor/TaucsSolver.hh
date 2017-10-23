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
//  CLASS TaucsSolver: 
//  direct solver for symmetric positive definite sparse systems
//
//=============================================================================


#ifndef TAUCS_SOLVER_HH
#define TAUCS_SOLVER_HH


//== INCLUDES =================================================================

#include <OpenMesh/Core/System/config.hh>
#include <vector>
#include <set>
#include <taucs.h>


//== CLASS DEFINITION =========================================================


class TaucsSolver
{
public:
   
  TaucsSolver();
  ~TaucsSolver();

  void begin_row();
  void add_value(int _i, double _val);
  void end_row();

  bool factorize(bool _use_supernodal=true);

  bool solve(std::vector<double>& _b, std::vector<double>& _x);

  template <class Vec>
  bool vec_solve(std::vector<Vec>& _b, std::vector<Vec>& _x);
  

private:

  void delete_matrices();


private:

  taucs_ccs_matrix           A, *PAP, *L;
  void                       *SL;
  std::vector<double>        values;
  std::vector<int>           colptr;
  std::vector<int>           rowind;
  int                        n_rows;
  int                        *perm, *invperm;
  bool                       supernodal_;
};


//=============================================================================
//#if defined(OM_INCLUDE_TEMPLATES) && !defined(TAUCS_SOLVER_C)
//#  define MB_TAUCS_SOLVER_TEMPLATES
//#  include "../../src/polygon-mesh-processor/TaucsSolver.cpp"
//#endif
//=============================================================================
#endif // TAUCS_SOLVER_HH defined
//=============================================================================
