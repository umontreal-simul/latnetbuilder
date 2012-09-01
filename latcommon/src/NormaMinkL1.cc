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

#include "latcommon/NormaMinkL1.h"

namespace LatCommon
{

const double NormaMinkL1::m_gamma[] =
{         //  GamMinkL1[i] = Lg(i!)
   /* GamMinkL1[0] = */    0.00000000000000,
   /* GamMinkL1[1] = */    0.00000000000000,
   /* GamMinkL1[2] = */    1.00000000000000,
   /* GamMinkL1[3] = */    2.58496250072116,
   /* GamMinkL1[4] = */    4.58496250072116,
   /* GamMinkL1[5] = */    6.90689059560852,
   /* GamMinkL1[6] = */    9.49185309632968,
   /* GamMinkL1[7] = */    12.29920801838728,
   /* GamMinkL1[8] = */    15.29920801838728,
   /* GamMinkL1[9] = */    18.46913301982959,
   /* GamMinkL1[10] = */    21.79106111471695,
   /* GamMinkL1[11] = */    25.25049273335425,
   /* GamMinkL1[12] = */    28.83545523407540,
   /* GamMinkL1[13] = */    32.53589495221650,
   /* GamMinkL1[14] = */    36.34324987427410,
   /* GamMinkL1[15] = */    40.25014046988262,
   /* GamMinkL1[16] = */    44.25014046988262,
   /* GamMinkL1[17] = */    48.33760331113297,
   /* GamMinkL1[18] = */    52.50752831257527,
   /* GamMinkL1[19] = */    56.75545582601886,
   /* GamMinkL1[20] = */    61.07738392090622,
   /* GamMinkL1[21] = */    65.46970134368497,
   /* GamMinkL1[22] = */    69.92913296232229,
   /* GamMinkL1[23] = */    74.45269491837929,
   /* GamMinkL1[24] = */    79.03765741910044,
   /* GamMinkL1[25] = */    83.68151360887516,
   /* GamMinkL1[26] = */    88.38195332701626,
   /* GamMinkL1[27] = */    93.13684082917975,
   /* GamMinkL1[28] = */    97.94419575123734,
   /* GamMinkL1[29] = */    102.80217674636492,
   /* GamMinkL1[30] = */    107.70906734197342,
   /* GamMinkL1[31] = */    112.66326365236030,
   /* GamMinkL1[32] = */    117.66326365236031,
   /* GamMinkL1[33] = */    122.70765777171876,
   /* GamMinkL1[34] = */    127.79512061296910,
   /* GamMinkL1[35] = */    132.92440362991405,
   /* GamMinkL1[36] = */    138.09432863135638,
   /* GamMinkL1[37] = */    143.30378199698532,
   /* GamMinkL1[38] = */    148.55170951042891,
   /* GamMinkL1[39] = */    153.83711172929117,
   /* GamMinkL1[40] = */    159.15903982417851,
   /* GamMinkL1[41] = */    164.51659182879661,
   /* GamMinkL1[42] = */    169.90890925157535,
   /* GamMinkL1[43] = */    175.33517400627747,
   /* GamMinkL1[44] = */    180.79460562491477,
   /* GamMinkL1[45] = */    186.28645872124443,
   /* GamMinkL1[46] = */    191.81002067730145,
   /* GamMinkL1[47] = */    197.36460952897906,
   /* GamMinkL1[48] = */    202.94957202970022
};



/*=========================================================================*/


NormaMinkL1::NormaMinkL1 (const MScal & m, int k, int t,
                          double beta)
      : Normalizer (m, k, t, "MinkL1", L1NORM, beta)
{
   if (t > MAX_DIM) //fred
      throw std::invalid_argument("NormaMinkL1:   dimension > MAX_DIM"); //fred

   Normalizer::init (m, k, beta);
}


/*=========================================================================*/


inline double NormaMinkL1::getGamma (int j) const throw(std::out_of_range)
{
   if (j < 1 || j > MAX_DIM)
      throw std::out_of_range("NormaMinkL1::getGamma");
   return m_gamma[j];
}

/*=======================================================================*/

}
