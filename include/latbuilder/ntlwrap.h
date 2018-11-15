// This file is part of LatticeTester.
//
// LatticeTester
// Copyright (C) 2012-2018  Pierre L'Ecuyer and Universite de Montreal
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef LATBUILDER__NTLWRAP_H
#define LATBUILDER__NTLWRAP_H

#include <NTL/vector.h>
#include <NTL/matrix.h>
#include <NTL/GF2E.h>

#include "latticetester/ntlwrap.h"


/**
 * The two floowing two classes are copies from NTL::Vec<T> and NTL::Mat<T>, but
 * they contain additional member functions having same names to the ones 
 * used in boost library.
 * This name conversion is meant to have the samee function names in boost and NTL 
 */

namespace NTL
{
   /**
   * comparing NTL::GF2X objects
   */
   inline bool operator< (const GF2X& x, const GF2X& y){ 
   
         if(deg(x)!= deg(y)){
            return (deg(x) < deg(y));
         }
         for(long i= deg(x); i>=0; i--){
            if(coeff(x,i) != coeff(y,i) )
                  return IsOne(coeff(y,i));

         }
         return false;
   }

   /**
   * comparing NTL::GF2E objects
   */
   inline bool operator< (const GF2E& x, const GF2E& y){ 
         GF2X xx = rep(x);
         GF2X yy = rep(y);
         return (xx < yy) ;
   }
}

#endif // __NTLWRAP_H__
