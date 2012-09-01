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

#include "latcommon/NormaLaminated.h"

namespace LatCommon
{

const double NormaLaminated::m_gamma[] =
   {
      /* Gamma[0] = */    0.00000000000000,
      /* Gamma[1] = */    1.00000000000000,
      /* Gamma[2] = */    1.15470053837925,
      /* Gamma[3] = */    1.25992104989487,
      /* Gamma[4] = */    1.41421356237309,
      /* Gamma[5] = */    1.51571656651040,
      /* Gamma[6] = */    1.66536635531121,
      /* Gamma[7] = */    1.81144732852781,
      /* Gamma[8] = */    2.00000000000000,
      /* Gamma[9] = */    2.00000000000000,
      /* Gamma[10] = */    2.05837201792952,
      /* Gamma[11] = */    2.13008217887993,
      /* Gamma[12] = */    2.24492409661875,
      /* Gamma[13] = */    2.34692092000925,
      /* Gamma[14] = */    2.48864391982238,
      /* Gamma[15] = */    2.63901582154579,
      /* Gamma[16] = */    2.82842712474619,
      /* Gamma[17] = */    2.88668115405991,
      /* Gamma[18] = */    2.98682599936104,
      /* Gamma[19] = */    3.09851928453331,
      /* Gamma[20] = */    3.24900958542494,
      /* Gamma[21] = */    3.39145596751014,
      /* Gamma[22] = */    3.57278019514216,
      /* Gamma[23] = */    3.76602735259556,
      /* Gamma[24] = */    4.00000000000000,
      /* Gamma[25] = */    3.89061978964914,
      /* Gamma[26] = */    3.83450381188673,
      /* Gamma[27] = */    3.79980642833440,
      /* Gamma[28] = */    3.80678061204248,
      /* Gamma[29] = */    3.81328532378654,
      /* Gamma[30] = */    3.85616802789424,
      /* Gamma[31] = */    3.91155414534173,
      /* Gamma[32] = */    4.00000000000000,
      /* Gamma[33] = */    4.00000000000000,
      /* Gamma[34] = */    4.03398853947441,
      /* Gamma[35] = */    4.08000643768480,
      /* Gamma[36] = */    4.15703690412737,
      /* Gamma[37] = */    4.23124164832276,
      /* Gamma[38] = */    4.33546037046114,
      /* Gamma[39] = */    4.45012590438595,
      /* Gamma[40] = */    4.59479341998814,
      /* Gamma[41] = */    4.65735933059734,
      /* Gamma[42] = */    4.75016320908659,
      /* Gamma[43] = */    4.85364895344187,
      /* Gamma[44] = */    4.98703323713956,
      /* Gamma[45] = */    5.11791276058506,
      /* Gamma[46] = */    5.27922773469898,
      /* Gamma[47] = */    5.45208672393488,
      /* Gamma[48] = */    5.65685424949238
   };



/*=========================================================================*/


NormaLaminated::NormaLaminated (const MScal & m, int k, int t, double beta)
      : Normalizer (m, k, t, "Laminated", L2NORM, beta)
{
   if (t > MAX_DIM)
      throw std::invalid_argument("NormaLaminated:   dimension > MAX_DIM");
   Normalizer::init (m, k, beta);
}


/*=========================================================================*/


inline double NormaLaminated::getGamma (int j) const
throw(std::out_of_range)
{
   if (j < 1 || j > MAX_DIM) //fred
      throw std::out_of_range("NormaLaminated::getGamma");
   return m_gamma[j];
}

/*=======================================================================*/

}
