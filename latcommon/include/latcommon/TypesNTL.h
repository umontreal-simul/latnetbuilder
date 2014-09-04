// Copyright (c) 2014 David Munger, Pierre L'Ecuyer, Université de Montréal.  
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
// 
// 
// Converted from tcode to Doxygen in 2014.

/**
 * 
 * 
 * Defines \c typedef's for the basic types used in LatMRG.%
 * 
 * Uses vectors and matrices implemented in the library NTL
 * (see the NTL web page at \\url{http://shoup.net/ntl/}).
 * Depending on how \c TYPAGE is defined, all types used will be either
 * primitives like \c long, \c double, etc., or the large number
 * types defined in NTL.
 * The preprocessor variable \c TYPAGE is defined in the Makefile.
 * 
 * 
 * \todo Richard: David aimerait remplacer <tt>NScal, NVect</tt>, \c NMat,
 * <tt>RScal, RVect</tt>, \c RMat,
 * <tt>MScal, MVect</tt>, \c MMat,
 * <tt>BScal, BVect</tt>, \c BMat par des noms plus significatifs
 * (il sera plus facile de savoir \`a quoi ils servent).
 * 
 */

#ifndef TYPESNTL_H
#define TYPESNTL_H

/**
 */
#include <NTL/vector.h>
#include <NTL/matrix.h>


#if TYPAGE == 1
// the case  "LLDD"

#include "latcommon/vec_lzz.h"
#include "latcommon/mat_lzz.h"
#include "NTL/lzz_p.h"
#include "NTL/vec_lzz_p.h"
#include "NTL/mat_lzz_p.h"
#include "latcommon/vec_double.h"
#include "latcommon/mat_double.h"
#include "NTL/lzz_pX.h"
#include "NTL/lzz_pE.h"
#include "NTL/lzz_pEX.h"
#include "latcommon/ntlwrap.h"

using NTL::matrix_row;
using namespace NTL;


typedef long              MScal;
typedef NTL::vector<long> MVect;
typedef NTL::matrix<long> MMat;
   typedef zz_p     MScalP;
   typedef vec_zz_p MVectP;
   typedef mat_zz_p MMatP;
typedef long                BScal;
typedef NTL::vector<long> BVect;
typedef NTL::matrix<long> BMat;
typedef double              NScal;
typedef NTL::vector<double> NVect;
typedef NTL::matrix<double> NMat;
typedef double              RScal;
typedef NTL::vector<double> RVect;
typedef NTL::matrix<double> RMat;
   typedef zz_pX    PolX;
   typedef zz_pE    PolE;


/**
 */
#elif TYPAGE == 2
// the case  "ZZDD"

#include "NTL/ZZ.h"
#include "NTL/vec_ZZ.h"
#include "NTL/mat_ZZ.h"
#include "latcommon/vec_double.h"
#include "latcommon/mat_double.h"
#include "NTL/ZZ_p.h"
#include "NTL/vec_ZZ_p.h"
#include "NTL/mat_ZZ_p.h"
#include "NTL/ZZ_pE.h"
#include "NTL/ZZ_pX.h"
#include "NTL/ZZ_pEX.h"
#include "latcommon/ntlwrap.h"

using NTL::matrix_row;
using namespace NTL;


typedef ZZ              MScal;
typedef NTL::vector<ZZ> MVect;
typedef NTL::matrix<ZZ> MMat;
   typedef ZZ_p     MScalP;
   typedef vec_ZZ_p MVectP;
   typedef mat_ZZ_p MMatP;
typedef ZZ              BScal;
typedef NTL::vector<ZZ> BVect;
typedef NTL::matrix<ZZ> BMat;
typedef double              NScal;
typedef NTL::vector<double> NVect;
typedef NTL::matrix<double> NMat;
typedef double              RScal;
typedef NTL::vector<double> RVect;
typedef NTL::matrix<double> RMat;
   typedef ZZ_pX    PolX;
   typedef ZZ_pE    PolE; 


/**
 */
#elif TYPAGE == 3
// the case  "ZZRR"

#include "NTL/ZZ.h"
#include "NTL/vec_ZZ.h"
#include "NTL/mat_ZZ.h"
#include "NTL/RR.h"
#include "NTL/vec_RR.h"
#include "NTL/mat_RR.h"
#include "NTL/ZZ_p.h"
#include "NTL/vec_ZZ_p.h"
#include "NTL/mat_ZZ_p.h"
#include "NTL/ZZ_pE.h"
#include "NTL/ZZ_pX.h"
#include "NTL/ZZ_pEX.h"
#include "latcommon/ntlwrap.h"

using NTL::matrix_row;
using namespace NTL;


typedef ZZ              MScal;
typedef NTL::vector<ZZ> MVect;
typedef NTL::matrix<ZZ> MMat;
   typedef ZZ_p     MScalP;
   typedef vec_ZZ_p MVectP;
   typedef mat_ZZ_p MMatP;
typedef ZZ              BScal;
typedef NTL::vector<ZZ> BVect;
typedef NTL::matrix<ZZ> BMat;
typedef RR              NScal;
typedef NTL::vector<RR> NVect;
typedef NTL::matrix<RR> NMat;
typedef RR              RScal;
typedef NTL::vector<RR> RVect;
typedef NTL::matrix<RR> RMat;
   typedef ZZ_pX    PolX;
   typedef ZZ_pE    PolE;


#else

#error "TYPAGE is not defined"
#endif



namespace LatMRG {

   typedef void ProcII (int, int);
}

#endif

