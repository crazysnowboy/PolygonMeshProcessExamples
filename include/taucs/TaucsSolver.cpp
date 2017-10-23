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
//  CLASS TaucsSolver - IMPLEMENTATION
//
//=============================================================================


#define TAUCS_SOLVER_C


//== INCLUDES =================================================================

#include "TaucsSolver.hh"
#include <iostream>


//== IMPLEMENTATION ==========================================================


#ifndef MB_TAUCS_SOLVER_TEMPLATES


//-----------------------------------------------------------------------------


TaucsSolver::TaucsSolver(): PAP(0), L(0), SL(0), n_rows(0), perm(0), invperm(0), supernodal_(true)
{
}


//-----------------------------------------------------------------------------


TaucsSolver::~TaucsSolver()
{
  delete_matrices();
}


//-----------------------------------------------------------------------------


void TaucsSolver::delete_matrices()
{
  if (PAP)      { taucs_ccs_free(PAP);               PAP     = 0; }
  if (L)        { taucs_ccs_free(L);                 L       = 0; }
  if (SL)       { taucs_supernodal_factor_free(SL);  SL      = 0; }
  if (perm)     { free(perm);                        perm    = 0; }
  if (invperm)  { free(invperm);                     invperm = 0; }
}


//-----------------------------------------------------------------------------


void TaucsSolver::begin_row()
{
  if (colptr.empty() || colptr.back() != (int)values.size())
  {
    colptr.push_back(values.size());
    n_rows = colptr.size()-1;
  }
}


void TaucsSolver::add_value(int _i, double _val)
{
  if (_i <= n_rows)
  {
    values.push_back(_val);
    rowind.push_back(_i);
  }
}


void TaucsSolver::end_row()
{
  if (colptr.empty() || colptr.back() != (int)values.size())
  {
    colptr.push_back(values.size());
    n_rows = colptr.size()-1;
  }
}


//-----------------------------------------------------------------------------


bool TaucsSolver::factorize(bool _use_supernodal)
{
  supernodal_ = _use_supernodal;


  // delete old matrices
  delete_matrices();


  // setup ccs matrix
  A.n        = colptr.size()-1;
  A.m        = colptr.size()-1;
  A.flags    = (TAUCS_DOUBLE | TAUCS_SYMMETRIC | TAUCS_LOWER);
  A.colptr   = &colptr[0];
  A.rowind   = &rowind[0];
  A.values.d = &values[0];


  // bandlimitation
  taucs_ccs_order(&A, &perm, &invperm, "metis");
  PAP = taucs_ccs_permute_symmetrically(&A, perm, invperm);
  if (!PAP) 
  { 
    std::cerr << "TaucsSolver: permutation failed\n";
    return false;
  }


  // Cholesky factorization
  if (supernodal_)  SL = taucs_ccs_factor_llt_mf (PAP);
  else               L = taucs_ccs_factor_llt    (PAP, 0, 0);
  if (!(L || SL)) 
  {
    std::cerr << "TaucsSolver: factorization failed\n";
    return false;
  }

  return true;
}


//-----------------------------------------------------------------------------


bool TaucsSolver::solve(std::vector<double>& _b, std::vector<double>& _x)
{
  const unsigned int N = A.n;

  if (N != _b.size() || N != _x.size())
  {
    std::cerr << "TaucsSolver: matrix size doesn't match vector size\n";
    return false;
  }


  std::vector<double>  PB(N), PX(N);


  // permute rhs
  for (unsigned int i=0; i<N; ++i)
    PB[i] = _b[perm[i]];


  // solve by back-substitution
  if ((supernodal_ ?
       taucs_supernodal_solve_llt(SL, &PX[0], &PB[0]) :
       taucs_ccs_solve_llt(L, &PX[0], &PB[0])) 
      != TAUCS_SUCCESS)
  {
    std::cerr << "TaucsSolver: back-substitution failed\n";
    return false;
  }


  // re-permute x
  for (unsigned int i=0; i<N; ++i)
    _x[i] = PX[invperm[i]];


  return true;
}

template <class Vec>
bool TaucsSolver::vec_solve(std::vector<Vec>& _b, std::vector<Vec>& _x)
{
  const unsigned int N = A.n;

  if (N != _b.size() || N != _x.size())
  {
    std::cerr << "TaucsSolver: matrix size doesn't match vector size\n";
    return false;
  }


  std::vector<double>  PB(N), PX(N);


  // solver component-wise
  for (int c=0; c<Vec::dim(); ++c)
  {
    // permute rhs
    for (unsigned int i=0; i<N; ++i)
      PB[i] = _b[perm[i]][c];


    // solve by back-substitution
    if ((supernodal_ ?
         taucs_supernodal_solve_llt(SL, &PX[0], &PB[0]) :
         taucs_ccs_solve_llt(L, &PX[0], &PB[0]))
        != TAUCS_SUCCESS)
    {
      std::cerr << "TaucsSolver: back-substitution failed\n";
      return false;
    }


    // re-permute x
    for (unsigned int i=0; i<N; ++i)
      _x[i][c] = PX[invperm[i]];
  }

  return true;
}


//=============================================================================
#endif // MB_TAUCS_SOLVER_TEMPLATES
//=============================================================================
