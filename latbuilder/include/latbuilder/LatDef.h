// Copyright (c) 2012 David Munger, Pierre L'Ecuyer, Université de Montréal.
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
