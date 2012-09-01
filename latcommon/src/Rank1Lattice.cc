// Copyright (c) 2012 Richard Simard, Pierre L'Ecuyer, Université de Montréal.
// 
// This file is part of Lattice Builder.
// 
// Lattice Builder is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Lattice Builder is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with Lattice Builder.  If not, see <http://www.gnu.org/licenses/>.

#include "latcommon/UtilLC.h"
#include "latcommon/Rank1Lattice.h"
#include <cassert> 
// #include <boost/numeric/ublas/matrix.hpp>
// #include <boost/numeric/ublas/matrix_proxy.hpp>

#ifdef WITH_NTL
#else
using namespace boost::numeric::ublas;
#endif

using namespace std;


namespace LatCommon
{

Rank1Lattice::Rank1Lattice (const MScal & n, const MVect & a, int maxDim,
                           NormType norm):
                  IntLattice::IntLattice (n, maxDim, maxDim, norm)
{
   m_a.resize (1 + maxDim);
   m_a = a;
   init ();
}


//=========================================================================

Rank1Lattice::~Rank1Lattice ()
{
   m_a.clear ();
}


//=========================================================================

void Rank1Lattice::init ()
{
   IntLattice::init ();
   for (int r = 2; r <= getMaxDim (); r++)
      m_lgVolDual2[r] = m_lgVolDual2[r - 1];
}


//=========================================================================

Rank1Lattice & Rank1Lattice::operator= (const Rank1Lattice & lat)
{
   if (this == &lat)
      return * this;
   copy (lat);
   init ();
   int maxDim = lat.getMaxDim ();
   m_a.resize (1 + maxDim);
   m_a = lat.m_a;
   return *this;
}


//=========================================================================

Rank1Lattice::Rank1Lattice (const Rank1Lattice & lat):
      IntLattice::IntLattice (lat.m_m, lat.getOrder (),
                              lat.getMaxDim (), lat.getNorm ())
{
   // MyExit (1, "Rank1Lattice:: constructeur n'est pas terminé " );
   init ();
   int maxDim = lat.getMaxDim ();
   m_a.resize (1 + maxDim);
   m_a = lat.m_a;
}


//===========================================================================

std::string Rank1Lattice::toStringCoef ()const
{
   return toString (m_a, 1, getMaxDim ());
}


//=========================================================================

void Rank1Lattice::incDim ()
{
   // kill();
   buildBasis (1 + getDim ());
   m_v.setNegativeNorm (true);
   m_w.setNegativeNorm (true);
}


//===========================================================================

void Rank1Lattice::buildBasis (int d)
{
   // assert(d <= getMaxDim());
   setDim (d);

   // conv(m_v[1][1], 1);

   for (int j = 1; j <= d; j++) {
      m_v (1, j) = m_a[j];
   }

   for (int i = 2; i <= d; i++) {
      for (int j = 1; j <= d; j++) {
         if (i == j) {
            m_v (i, j) = m_m;
         } else {
            m_v (i, j) = 0;
         }
      }
   }

   // if a[1] != 1, the basis must be triangularized
   if (m_v (1, 1) != 1) {
      Triangularization < Base > (m_v, m_w, d, d, m_m);
      dualize ();
   }
   CalcDual < Base > (m_v, m_w, d, m_m);
   m_v.setNegativeNorm (true);
   m_w.setNegativeNorm (true);
}

}
