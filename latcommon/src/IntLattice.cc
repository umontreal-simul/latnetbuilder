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
#include "latcommon/IntLattice.h"

#include "latcommon/NormaBestLat.h"
#include "latcommon/NormaLaminated.h"
#include "latcommon/NormaRogers.h"
#include "latcommon/NormaMinkL1.h"
#include "latcommon/NormaMinkowski.h"

#include <fstream>
#include <cassert>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <string>
#include <typeinfo>
/*
#include <limits>
#include <cstdlib>
*/

#ifdef WITH_NTL
#include "NTL/ZZ_p.h"
#include "NTL/lzz_p.h"
#include "NTL/mat_ZZ.h"
#include "NTL/tools.h"
#include "NTL/ZZ.h"
#include "latcommon/vec_lzz.h"
#include "NTL/quad_float.h"
#include "NTL/RR.h"
#include "latcommon/UtilLM.h"
using namespace NTL;

#else
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
using namespace boost::numeric::ublas;

#endif

using namespace std;

#define SEPAR "===============================================================\n"

namespace LatCommon
{

//===========================================================================

void IntLattice::trace (char *mess)
{
   cout << "-----------------------------------------------------\n"
        << mess << "  **********************" << endl;
   cout << "dim = " << m_v.getDim () << endl;
   m_v.write();
   m_w.write();
   checkDuality ();
}


//=========================================================================

IntLattice::IntLattice (const MScal & m0, int k, int maxDim, NormType norm0):
            m_order (k), m_m (m0),
            m_v (k, maxDim, norm0),
            m_w (k, maxDim, norm0),
            m_lgVolDual2(0), m_xx(0),
            m_vTemp(m_v)
{
   conv (m_m2, m_m);
   m_m2 = m_m2 * m_m2;

#ifdef WITH_NTL
   // Squared length of vectors must not overflow max(double)
   if ((norm0 == L2NORM) && ((typeid (m_m) == typeid (double)) ||
                             (typeid (m_m) == typeid (quad_float)))) {
      NTL::RR mx;
      conv (mx, m_m);
      if (mx > 3.3e150)        // 2^500
         assert (typeid (m_m2) != typeid (double) &&
                 typeid (m_m2) != typeid (quad_float));
   }
#endif
   init ();
}


//=========================================================================

IntLattice::IntLattice (const IntLattice & lat):
            m_v (lat.m_order, lat.getMaxDim (), lat.getNorm ()),
            m_w (lat.m_order, lat.getMaxDim (), lat.getNorm ()),
            m_lgVolDual2(0), m_xx(0), m_vTemp (m_v)
{
   copy (lat);
}


//=========================================================================

void IntLattice::init ()
{
   kill ();
   double temp;
   conv (temp, m_m);

   const int maxDim = getMaxDim ();
   m_lgVolDual2 = new double[1 + maxDim];
   m_lgm2 = 2.0 * log2 (temp);
   calcLgVolDual2 (true);      // May be different in subclasses
   m_xx = new bool[1 + maxDim];
   for (int i = 0; i <= maxDim; i++)
      m_xx[i] = true;
   m_vSI.resize(1 + maxDim, 1 + maxDim);
}


//=========================================================================

void IntLattice::kill ()
{
   if (m_lgVolDual2 == 0)
      return;
   delete [] m_lgVolDual2;
   m_lgVolDual2 = 0;
   delete [] m_xx;
   m_xx = 0;
   m_vSI.clear();
}


//=========================================================================

IntLattice::~IntLattice ()
{
   kill ();
   m_v.clear ();
   m_w.clear ();
   m_vTemp.clear ();
}


//=========================================================================

IntLattice & IntLattice::operator= (const IntLattice & lat)
{
   if (this == &lat)
      return *this;

   copy (lat);
   return *this;
}


//=========================================================================

void IntLattice::copy (const IntLattice & lat)
{
   m_order = lat.getOrder();
   m_m = lat.m_m;
   m_m2 = lat.m_m2;
   m_v = lat.m_v;
   m_w = lat.m_w;
   init ();
   for (int i = 1; i <= lat.getDim (); i++)
      m_xx[i] = lat.getXX(i);
}


//=========================================================================

void IntLattice::setDim (int d)
{
   assert (d > 0);

   int max = getMaxDim ();
   if (d > max) {
      d = max;
   }

   m_v.setDim (d);
   m_w.setDim (d);
}


//=========================================================================

void IntLattice::incDim ()
{
   MyExit(1, " incDim does nothing");
}


//=========================================================================

void IntLattice::buildBasis (int d)
{
   MyExit(1, " buildBasis does nothing");
   d++;  // eliminates compiler warning
}


//=========================================================================

void IntLattice::permute (int i, int j)
{
   if (i == j)
      return ;
   m_v.permute (i, j);
   m_w.permute (i, j);

   bool b = m_xx[j];
   m_xx[j] = m_xx[i];
   m_xx[i] = b;
}


//=========================================================================

void IntLattice::calcLgVolDual2 (bool dualF)
{
   int maxDim = getMaxDim();
   int rmax = min(m_order, maxDim);

   m_lgVolDual2[1] = m_lgm2;
   for (int r = 2; r <= rmax; r++)
      m_lgVolDual2[r] = m_lgVolDual2[r - 1] + m_lgm2;
   for (int r = rmax + 1; r <= maxDim; r++)
      m_lgVolDual2[r] = m_lgVolDual2[r - 1];

   if (!dualF) {
      // Normalization factor: dual to primal : M^(k/dim) -> 1/M^(k/dim)
      for (int i = 1; i <= maxDim; i++)
         m_lgVolDual2[i] = -m_lgVolDual2[i];
   }
}


//=========================================================================

void IntLattice::dualize ()
{
     m_vTemp = m_v;   m_v = m_w;   m_w = m_vTemp;
//   m_v.swap (m_w);
}


//=========================================================================

bool IntLattice::checkDuality ()
{
   BScal S;
   int dim = getDim ();

   for (int i = 1; i <= dim; i++) {
      for (int j = 1; j <= dim; j++) {
         matrix_row<Base> row1(m_v, i);
         matrix_row<Base> row2(m_w, j);
         ProdScal (row1, row2, dim, S);
         if (j != i) {
            if (S != 0) {
               cout << "******  checkDuality failed for V[" << i <<
                    "] and W[" << j << "]" << endl;
               return false;
            }
         } else if (S != m_m) {
            cout << "******  checkDuality failed for i, j = " << i << " , " <<
                     j << endl;
            return false;
         }
      }
   }
   return true;
}


//=========================================================================

bool IntLattice::baseEquivalence (IntLattice & lat)
{
   if (getDim () != lat.getDim ()) {
      return false;
   }

   int d = getDim ();
   BScal R, Q;

   for (int i = 1; i <= d; i++) {
      for (int j = 1; j <= d; j++) {
         matrix_row<Base> row1(m_v, i);
         matrix_row<Base> row2(m_w, j);
         ProdScal (row1, row2, d, R);
         conv (Q, lat.m_m);
         Divide (Q, R, R, Q);
         if (R != 0) {
            return false;
         }
         matrix_row<Base> row3(lat.m_v, i);
         matrix_row<Base> row4(lat.m_w, j);
         ProdScal (row3, row4, d, R);
         conv (Q, m_m);
         Divide (Q, R, R, Q);
         if (R != 0) {
            return false;
         }
      }
   }
   return true;
}


//===========================================================================

string IntLattice::toStringCoef () const
{
   assert (0);
   return std::string();
}


//=========================================================================

void IntLattice::write (int flag)
{
   int i;
   /*
     cout << SEPAR << "\n   ";
     cout << dim << "                 <-- Dim\n";
     cout << setprecision (15) << m << "         <-- m\n\n";
     for (i = 1; i <= dim; i++) {
     for (int j = 1; j <= dim; j++) {
     cout << setprecision (15) << V[i][j] << "   "
     << setprecision (15) << W[i][j]
     << "         <-- V[" << i << "," << j << "],     W["
     << i << "," << j << "]\n";
     }
     cout << "\n";
     }
   */
   int dim = getDim ();

   cout << SEPAR << "\n";
   for (i = 1; i <= dim; i++) {
      for (int j = 1; j <= dim; j++) {
         cout << setprecision (2) << 
         setw (6) << m_v(i,j) << "   ";
      }
      cout << "| |";
      for (int j = 1; j <= dim; j++) {
         cout << setprecision (2) << 
         setw (6) << m_w(i,j) << "    ";
      }
      cout << endl;
   }
   cout << endl;

   if (flag) {
      m_v.updateVecNorm ();
      m_w.updateVecNorm ();
   }

   for (i = 1; i <= dim; i++) {
      if (m_v.isNegativeNorm (i))
         cout << setprecision (15) << " -1";
      else
         cout << setprecision (15) << m_v.getVecNorm (i);
      cout << "    ";
      if (m_w.isNegativeNorm (i))
         cout << setprecision (15) << " -1";
      else
         cout << setprecision (15) << m_w.getVecNorm (i);
      cout << "      <-- VV[" << i << "],     WW[" << i << "]\n";
   }
   cout << endl;
   /*
     NScal x;
     for (i = 1; i <= dim; i++) {
     ProdScal (V[i], V[i], dim, x);
     if (x != V.vecNorm[i] && V.isNegativeNorm[i])
     cout << " *** ERREUR:   negative norm en V.vecNorm[" << i <<"] = " << V.vecNorm[i] << i << endl;
     ProdScal (W[i], W[i], dim, x);
     if (x != W.vecNorm[i] && W.isNegativeNorm[i])
     cout << " *** ERREUR:   negative norm en W.vecNorm[" << i <<"] = " << W.vecNorm[i] << i << endl;
     }
   */
}


//========================================================================

void IntLattice::write (const char *fname) const
{
   ofstream out (fname);
   if (!out) {
      string str ("IntLattice<>::Write:  stream not opened for file ");
      str += fname;
      throw invalid_argument (str);
   }

   int dim = getDim ();

   out << SEPAR << "IntLattice\n   ";
   out << dim << "                 <-- Dim\n";
   out << setprecision (15) << m_m << "         <-- m\n\n";
   for (int i = 1; i <= dim; i++) {
      for (int j = 1; j <= dim; j++) {
         out << setprecision (15) << m_v(i,j) << "   "
         << setprecision (15) << m_w(i,j)
         << "         <-- V[" << i << "," << j << "],     W["
         << i << "," << j << "]\n";
      }
      out << "\n";
   }
}


//=========================================================================

void IntLattice::sort (int d)
/*
 * We assume that the square lengths are already updated.
 * This gives flexibility to the user to put something else than
 * the square Euclidean length in V.vecNorm, W.vecNorm, etc.
 */
{
   if (m_v.isNegativeNorm (d + 1))
      m_v.updateVecNorm ();

   int dim = getDim ();
   if (true) {
      for (int i = 1; i <= dim; i++)
         if (m_v.getVecNorm(i) < 0) {
            cout << "\n***** ERROR:   Negative norm for i = " << i <<
               ",  dim = " << dim << endl;
         }
   }

   for (int i = d + 1; i < dim; i++)
   {
      int k = i;
      for (int j = i + 1; j <= dim; j++) {
         if (m_v.getVecNorm (j) < m_v.getVecNorm (k))
            k = j;
      }
      if (i != k)
         permute (i, k);
   }
}


//=========================================================================

Normalizer * IntLattice::getNormalizer (NormaType norma, int alpha)
{
   int dim = m_v.getMaxDim();
   Normalizer *normal;

   switch (norma) {
   case BESTLAT:
      normal = new NormaBestLat (m_m, m_order, dim);
      break;
   case LAMINATED:
      normal = new NormaLaminated (m_m, m_order, dim);
      break;
   case ROGERS:
      normal = new NormaRogers (m_m, m_order, dim);
      break;
   case MINKL1:
      normal = new NormaMinkL1 (m_m, m_order, dim);
      break;
   case MINKOWSKI:
      normal = new NormaMinkowski (m_m, m_order, dim);
      break;
   case NORMA_GENERIC:
      normal = new Normalizer (m_m, m_order, dim, "Norma_generic");
      break;
   case PALPHA_N:
      //  normal = new NormaPalpha (m_m, alpha, dim);
      //  break;
   default:
      cout << "normalizer:   no such case";
      exit (2);
   }
   return normal;
}


//=========================================================================

void IntLattice::buildProjection (IntLattice* lattice, const Coordinates & proj)
{
   const int dim = getDim ();
//  cout << "      ESPION_2\n";  getPrimalBasis ().write();
   int i = 0;
   for (Coordinates::const_iterator iter = proj.begin();
        iter != proj.end(); ++iter) {
      for (int j = 1; j <= dim; j++)
         lattice->m_w(j,i + 1) = m_v(j, *iter);
      ++i;
   }

   lattice->setDim (static_cast<int>(proj.size()));
   lattice->m_order = m_order;

   Triangularization<Base> (lattice->m_w, lattice->m_v, dim, static_cast<int>(proj.size()), m_m);
// lattice->trace("\nESPION_4");
/* cout << "  ***** build 2\n";
lattice->getPrimalBasis ().setNegativeNorm (true);
lattice->getPrimalBasis ().updateScalL2Norm (1,proj.size());
lattice->getPrimalBasis ().write();*/
   CalcDual<Base> (lattice->m_v, lattice->m_w, static_cast<int>(proj.size()), m_m);
/*
cout << "  ***** build 3\n";
lattice->getDualBasis ().setNegativeNorm (true);
lattice->getDualBasis ().updateScalL2Norm (1,proj.size());
lattice->getDualBasis ().write();
*/
   lattice->getPrimalBasis ().setNegativeNorm (true);
   lattice->getDualBasis ().setNegativeNorm (true);
}

}
