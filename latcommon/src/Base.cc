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

#include <iomanip>
#include <cassert>
#include <ostream>

#include "latcommon/Types.h"
#include "latcommon/Const.h"
#include "latcommon/Base.h"
#include "latcommon/UtilLC.h"


#ifdef WITH_NTL
#include "latcommon/UtilLM.h"
using namespace NTL;

#else
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/vector.hpp>
using namespace boost::numeric::ublas;

#endif

using namespace std;


namespace LatCommon
{

//===========================================================================

Base::Base (int d, int maxDim, NormType norm)
{
   assert (d > 0);
   assert (maxDim > 0);

   if (d > maxDim)
      d = maxDim;

   m_dim = d;
   m_maxDim = maxDim;
   m_norm = norm;

   BMat::resize (1 + maxDim, 1 + maxDim);
   m_vecNorm.resize (1 + maxDim);
   m_negFlag = new bool[1 + maxDim];
   for (int i = 0; i <= maxDim; i++) {
      m_negFlag[i] = true;
      for (int j = 0; j <= maxDim; j++) {
         (*this)(i,j) = 0;
      }
   }
}


/*=========================================================================*/

Base::Base (const Base & base): BMat (base)
{
   m_dim = base.m_dim;
   m_maxDim = base.m_maxDim;
   BMat::operator=(base);
   m_vecNorm = base.m_vecNorm;

   int d = m_maxDim + 1;
   m_negFlag = new bool[d];
   for (int i = 0; i < d; i++)
      m_negFlag[i] = base.m_negFlag[i];
   m_norm = base.m_norm;
}


/*=========================================================================*/

Base::~Base ()
{
   kill ();
}


/*=========================================================================*/

void Base::kill ()
{
   if (m_negFlag != 0) {
      delete[] m_negFlag;
      m_negFlag = 0;
   }
   m_vecNorm.clear ();
   BMat::clear (); 
}

/*=========================================================================*/

Base & Base::operator= (const Base & base)
{
   if (this == &base)
      return * this;
   kill();
   m_norm = base.m_norm;
   m_dim = base.m_dim;
   m_maxDim = base.m_maxDim;
   BMat::operator=(base);
   m_vecNorm = base.m_vecNorm;
   m_negFlag = new bool[m_maxDim + 1];
   for (int i = 0; i <= m_dim; i++)
      m_negFlag[i] = base.m_negFlag[i];

   return *this;
}


/*=========================================================================*/

void Base::permute (int i, int j)
{
   if (i == j)
      return ;
   for (int k = 1; k <= m_dim; k++) {
      LatCommon::swap9 ((*this)(j,k), (*this)(i,k));
   }
   LatCommon::swap9 (m_vecNorm[i], m_vecNorm[j]);
   LatCommon::swap9 (m_negFlag[i], m_negFlag[j]);
}


/*=========================================================================*/

void Base::setDim (int d)
{
   assert (d > 0);

   if (d > m_maxDim) {
      m_dim = m_maxDim;
   } else {
      m_dim = d;
   }
}


/*=========================================================================*/

void Base::setNorm (NormType norm)
{
   if (m_norm == norm)
      return ;
   m_norm = norm;
   setNegativeNorm (true);
}


/*=========================================================================*/

void Base::setVecNorm (NScal & value, int i)
{
   //   assert (value >= 0);
   //   assert (i > 0 && i <= m_maxDim);
   m_vecNorm[i] = value;
   m_negFlag[i] = false;
}


/*=========================================================================*/

void Base::swap (Base & b)
{
   LatCommon::swap9 (m_dim, b.m_dim);
   LatCommon::swap9 (m_maxDim, b.m_maxDim);
   LatCommon::swap9 (m_vecNorm, b.m_vecNorm);
   LatCommon::swap9 (*this, b);
   LatCommon::swap9 (m_negFlag, b.m_negFlag);
   LatCommon::swap9 (m_norm, b.m_norm);
}


/*=========================================================================*/

void Base::updateVecNorm (int d)
{
   assert (d >= 0);

   for (int i = d + 1; i <= m_dim; i++) {
      if (m_negFlag[i]) {
         matrix_row<BMat> row(*this, i);
         if (m_norm == L2NORM) {
            ProdScal (row, row, m_dim, m_vecNorm[i]);
         } else {
            CalcNorm <BVect, NScal> (row, m_dim, m_vecNorm[i], m_norm);
         }

        m_negFlag[i] = false;
      }

      if (m_vecNorm[i] < 0) {
         cout << "\n***** ERROR:   Negative norm for i = " << i <<
         ",  dim = " << m_dim << endl;
         cout << "Basis vector:   ";
         matrix_row<BMat> row2(*this, i);
         cout << LatCommon::toString (row2, 0, m_dim, "  ") << endl;
         exit(1);
      }
   }
}


/*=========================================================================*/

void Base::updateVecNorm ()
{
   updateVecNorm (0);
}


/*=========================================================================*/

void Base::updateScalL2Norm (int i)
{
   if (m_negFlag[i]) {
      matrix_row<BMat> row(*this, i);
      ProdScal (row, row, m_dim, m_vecNorm[i]);
   }
   m_negFlag[i] = false;
   if (m_vecNorm[i] < 0) {
      cout << "\n***** ERROR:   Negative norm L2 for i = " << i <<
      ",  dim = " << m_dim << endl;
      cout << "Basis vector:   ";
      matrix_row<BMat> row2(*this, i);
      cout << LatCommon::toString (row2, 0, m_dim, "  ") << endl;
      exit(1);
   }
}


/*=========================================================================*/

void Base::updateScalL2Norm (int k1, int k2)
{
   for (int i = k1; i <= k2; i++) {
      if (m_negFlag[i]) {
         matrix_row<BMat> row(*this, i);
         ProdScal (row, row, m_dim, m_vecNorm[i]);

      }
      m_negFlag[i] = false;
      if (m_vecNorm[i] < 0) {
         cout << "\n***** ERROR:   Negative norm L2 for i = " << i <<
         ",  dim = " << m_dim << endl;
         cout << "Basis vector:   ";

         matrix_row<BMat> row2(*this, i);
         cout << LatCommon::toString (row2, 0, m_dim, "  ") << endl;
         exit(1);
      }
   }
}


/*=========================================================================*/

void Base::setNegativeNorm (bool flag)
{
   for (int i = 1; i <= m_dim; i++) {
      m_negFlag[i] = flag;
   }
}


/*=========================================================================*/

string Base::toString() const
{
   ostringstream os;
   os << "Basis:\n";
   os << "  Dim = " << m_dim << " \n";
   for (int i = 1; i <= m_dim; i++) {
      os << "    [";
      for (int j = 1; j <= m_dim; j++)
         os << " " <<  setprecision (15) << (*this)(i,j);
      os << " ]\n";
   }

   os << "  Norm:\n";
   for (int i = 1; i <= m_dim; i++) {
      os << "    ";
      if (m_negFlag[i]) {
         os << "-1" << endl;
      } else if (m_vecNorm[i] < 0) {
         os << "Erreur:  norm < 0 and negFlag not set" << endl;
      } else {
         os << m_vecNorm[i] << endl;
      }
   }
   os << endl;
   return os.str ();
}


/*=========================================================================*/

string Base::toString(int i) const
{
   ostringstream os;
   os << "Dim = " << m_dim << " \n";
   os << "   Base[" << i << "] =  [";
   for (int j = 1; j <= m_dim; j++)
      os << "  " << (*this)(i,j);
   os << " ]\n";

   if (L2NORM == m_norm)
      os << "   Length^2 =   ";
   else
      os << "   Length =   ";
   if (m_negFlag[i]) {
      os << "-1" << endl;
   } else if (m_vecNorm[i] < 0) {
      os << "Erreur:  norm < 0 and negFlag not set" << endl;
   } else {
      os << setprecision (15) << m_vecNorm[i] << endl;
      NScal x = 1.0 / m_vecNorm[i];
      if (L2NORM == m_norm)
         x = sqrt(x);
      os << "   1/Length =   " << x << endl;
   }
   os << endl;
   return os.str ();
}


/*=========================================================================*/

void Base::write () const
{
   cout << "Dim = " << m_dim << " \n";
   for (int i = 1; i <= m_dim; i++) {
      cout << "   | ";
      for (int j = 1; j <= m_dim; j++) {
         cout << setprecision (15) << (*this)(i,j) << "\t";
      }
      cout << " |" << endl;
   }
   // return;
   cout << "Norm:\n";

   for (int i = 1; i <= m_dim; i++) {
      cout << "   ";
      if (m_negFlag[i]) {
         cout << "-1" << endl;
      } else if (m_vecNorm[i] < 0) {
         cout << "Erreur:  norm < 0 and negFlag not set" << endl;
      } else {
         cout << m_vecNorm[i] << endl;
      }
   }
   cout << endl;
}


/*=========================================================================*/

void Base::write (int i) const
{
   cout << "Dim = " << m_dim << " \n";
   cout << "   Base[" << i << "] =  [ ";
   for (int j = 1; j <= m_dim; j++) {
      cout << setprecision (15) << (*this)(i,j) << "  ";
   }
   cout << "]" << endl;

   if (L2NORM == m_norm)
      cout << "   Length^2 =   ";
   else
      cout << "   Length =   ";
   if (m_negFlag[i]) {
      cout << "-1" << endl;
   } else if (m_vecNorm[i] < 0) {
      cout << "Erreur:  norm < 0 and negFlag not set" << endl;
   } else {
      cout << m_vecNorm[i] << endl;
      NScal x = 1.0 / m_vecNorm[i];
      if (L2NORM == m_norm)
         x = sqrt(x);
      cout << "   1/Length =   " << x << endl;
   }
   cout << endl;
}

}
