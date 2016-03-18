// This file is part of Lattice Builder.
//
// Copyright (C) 2012-2016  Pierre L'Ecuyer and Universite de Montreal
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

#ifndef LATBUILDER__LAT_DEF_H
#define LATBUILDER__LAT_DEF_H

#include "latbuilder/Types.h"
#include "latbuilder/SizeParam.h"
#include "latbuilder/TextStream.h"

namespace LatBuilder
{

/**
 * Definition of a rank-1 lattice.
 * Defining parameters:
 * - lattice size parameter;
 * - generating vector.
 *
 * \tparam LAT  Type of lattice.
 */
template <LatType LAT>
class LatDef {
public:
   /**
    * Constructor.
    * \param sizeParam     Size parameter of the lattice.
    * \param gen           Generating vector.
    */
   LatDef(
         SizeParam<LAT> sizeParam = SizeParam<LAT>(),
         GeneratingVector gen = GeneratingVector()
         ):
      m_sizeParam(std::move(sizeParam)),
      m_gen(std::move(gen))
   {}

   template <LatType L>
   LatDef(const LatDef<L>& other): LatDef(other.sizeParam(), other.gen())
   {}

   /**
    * Returns the size parameter of the lattice.
    */
   SizeParam<LAT>& sizeParam()
   { return m_sizeParam; }

   /// \copydoc sizeParam()
   const SizeParam<LAT>& sizeParam() const
   { return m_sizeParam; }

   /**
    * Returns the generating vector of the lattice.
    */
   GeneratingVector& gen()
   { return m_gen; }

   /// \copydoc gen()
   const GeneratingVector& gen() const
   { return m_gen; }

   /**
    * Returns the dimension of the lattice.
    */
   Dimension dimension() const
   { return gen().size(); }

   /**
    * Returns \c true this instance is equal to \c other.
    */
   bool operator== (const LatDef& other) const
   { return sizeParam() == other.sizeParam() and gen() == other.gen(); }

   /**
    * Returns \c true this instance is not equal to \c other.
    */
   bool operator!= (const LatDef& other) const
   { return not operator==(other); }

   /**
    * Returns \c true this instance has a smaller size than \c other, or if it
    * has the same size and a smaller lexicographic rank for its generating
    * vector than \c other.
    */
   bool operator< (const LatDef& other) const
   { return sizeParam() < other.sizeParam() and gen() < other.gen(); }


private:
   SizeParam<LAT> m_sizeParam;
   GeneratingVector m_gen;
};


/**
 * Formats \c lat and outputs it to \c os.
 */
template <LatType LAT>
std::ostream& operator<< (std::ostream& os, const LatDef<LAT>& lat)
{
   using TextStream::operator<<;
   os << "lattice(" << lat.sizeParam() << ", " << lat.gen() << ")";
   return os;
}

/**
 * Returns a lattice definition instance with the proper type of size parameter.
 */
template <LatType LAT>
LatDef<LAT> createLatDef(
      SizeParam<LAT> sizeParam = SizeParam<LAT>(), 
      GeneratingVector gen = GeneratingVector()
      )
{ return LatDef<LAT>(std::move(sizeParam), std::move(gen)); }

}

#endif
