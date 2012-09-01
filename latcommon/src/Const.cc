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

#include "latcommon/Const.h"
#include <string>


using namespace std;

namespace LatCommon
{

//===========================================================================

string toStringNorm (NormType norm)
{
   switch (norm) {
   case SUPNORM:
      return "SUPNORM";
   case L1NORM:
      return "L1NORM";
   case L2NORM:
      return "L2NORM";
   case ZAREMBANORM:
      return "ZAREMBANORM";
   default:
      return "***** NormType: IMPOSSIBLE CASE ";
   }
}


//===========================================================================

string toStringGen (GenType gen)
{
   switch (gen) {
   case MWC:
      return "MWC";
   case LCG:
      return "LCG";
   case MRG:
      return "MRG";
   case KOROBOV:
      return "KOROBOV";
   case RANK1:
      return "RANK1";
   default:
      return "***** GenType: IMPOSSIBLE CASE ";
   }
}


//===========================================================================

string toStringLattice (LatticeType lat)
{
   switch (lat) {
   case FULL:
      return "FULL";
   case RECURRENT:
      return "RECURRENT";
   case ORBIT:
      return "ORBIT";
   case PRIMEPOWER:
      return "PRIMEPOWER";
   default:
      return "***** LatticeType: IMPOSSIBLE CASE ";
   }
}


//===========================================================================

string toStringPrime (PrimeType stat)
{
   switch (stat) {
   case PRIME:
      return "PRIME";
   case PROB_PRIME:
      return "PROB_PRIME";
   case COMPOSITE:
      return "COMPOSITE";
   default:
      return "UNKNOWN";
   }
}


//===========================================================================

string toStringDecomp (DecompType deco)
{
   switch (deco) {
   case DECOMP:
      return "DECOMP";
   case DECOMP_WRITE:
      return "DECOMP_WRITE";
   case DECOMP_READ:
      return "DECOMP_READ";
   case DECOMP_PRIME:
      return "DECOMP_PRIME";
   default:
      return "***** DecompType: IMPOSSIBLE CASE ";
   }
}


//===========================================================================

string toStringOutput (OutputType sort)
{
   switch (sort) {
   case TERMINAL:
      return "TERMINAL";
   case RES:
      return "RES";
   case TEX:
      return "TEX";
   case GEN:
      return "GEN";
   default:
      return "***** OutputType: IMPOSSIBLE CASE ";
   }
}


//===========================================================================

string toStringCriterion (CriterionType criter)
{
   switch (criter) {
   case SPECTRAL:
      return "SPECTRAL";
   case BEYER:
      return "BEYER";
   case PALPHA:
      return "PALPHA";
   default:
      return "***** CriterionType: IMPOSSIBLE CASE ";
   }
}


//===========================================================================

string toStringNorma (NormaType norma)
{
   switch (norma) {
   case BESTLAT:
      return "BESTLAT";
   case LAMINATED:
      return "LAMINATED";
   case ROGERS:
      return "ROGERS";
   case MINKL1:
      return "MINKL1";
   case MINKOWSKI:
      return "MINKOWSKI";
   case NORMA_GENERIC:
      return "NORMA_GENERIC";
   default:
      return "***** NormaType: IMPOSSIBLE CASE ";
   }
}


//===========================================================================

string toStringCalc (CalcType calc)
{
   switch (calc) {
   case PAL:
      return "PAL";
   case NORMPAL:
      return "NORMPAL";
   case BAL:
      return "BAL";
   case SEEKPAL:
      return "SEEKPAL";
   default:
      return "***** CalcType: IMPOSSIBLE CASE ";
   }
}


//===========================================================================

string toStringImplemCond (ImplemCond cond)
{
   switch (cond) {
   case NO_COND:
      return "NO_COND";
   case APP_FACT:
      return "APP_FACT";
   case EQUAL_COEF:
      return "EQUAL_COEF";
   case ZERO_COEF:
      return "ZERO_COEF";
   case POWER_TWO:
      return "POWER_TWO";
   default:
      return "***** ImplemCond: IMPOSSIBLE CASE ";
   }
}


//===========================================================================

string toStringSearchMethod (SearchMethod method)
{
   switch (method) {
   case EXHAUST:
      return "EXHAUST";
   case RANDOM:
      return "RANDOM";
   default:
      return "***** SearchMethod: IMPOSSIBLE CASE ";
   }
}


//===========================================================================

}
