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


#ifdef WITH_NTL
#include "NTL/ZZ.h"
#include "NTL/tools.h"
#endif

#include <cassert>
#include <cstdlib>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <string>

#include "latcommon/Types.h"
#include "latcommon/Normalizer.h"


namespace LatCommon
{

const int Normalizer::MAX_DIM;

/*-------------------------------------------------------------------------*/

Normalizer::Normalizer (const MScal & m0, int k0, int maxDim, std::string name,
                        NormType norm, double beta0) :
      m_name(name), m_norm(norm), m_m(m0), m_rank(k0), m_maxDim(maxDim),
      m_beta(beta0)
{
   m_cst = new double[maxDim + 1];
}


/*-------------------------------------------------------------------------*/

void Normalizer::init (const MScal &m0, int k0, double beta0)
/*
 * Computes the vector Cst that corresponds to G, for a lattice of
 * density m^k for t >= k, and m^t for t < k.
 */
{
   double x, y;
   double logBeta;
   double logm;
   double k = k0;
   m_rank = k0;
   m_m = m0;
   m_beta = beta0;

   y = 1.0;
   logBeta = log (m_beta);
#ifdef WITH_NTL
   logm = log(NTL::to_ZZ(m_m));
#else
   logm = log(m_m);
#endif
   for (int j = 1; j <= m_maxDim; j++) { //fred
      if (j > k)
         y = k / j;
      x = log (getGamma(j)) + j * logBeta + y * logm;
      if (m_norm == L2NORM)
         x = x + x;
      m_cst[j] = exp (x);
   }
}


/*-------------------------------------------------------------------------*/

std::string Normalizer::ToString () const
{
   std::ostringstream os;
   os << "-----------------------------\n"
   << "Content of Normalizer object:\n\n Normalizer = " << m_name;
   os << "\n m = " << m_m;
   os << "\n rank = " << m_rank;
   os << "\n beta = " << std::setprecision (4) << m_beta << "\n\n";

   //   os.setf(std::ios::left);
   os << std::setprecision (13);
   for (int t = 1; t <= m_maxDim; t++) { //fred
      os << " Cst[" << std::setw(2) << std::right << t << "] = "
      << std::setw(14) << std::left << m_cst[t] << "\n";
   }
   os << "\n";
   return os.str ();
}


/*-------------------------------------------------------------------------*/

double Normalizer::getGamma (int) const
{
   return 1.0;
}


/*-------------------------------------------------------------------------*/

double & Normalizer::getCst (int j)
{
   assert (j >= 1 && j <= m_maxDim); //fred
   return m_cst[j];
}

}
