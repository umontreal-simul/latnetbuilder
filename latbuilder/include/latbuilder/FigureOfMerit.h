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

#ifndef LATBUILDER__FIGURE_OF_MERIT_H
#define LATBUILDER__FIGURE_OF_MERIT_H

#include "latbuilder/Types.h"
#include "latbuilder/LatDef.h"

#include "latcommon/Weights.h"

#include <ostream>

namespace LatBuilder
{

/**
 * Abstract base class for figures of merit.
 */
class FigureOfMerit {
public:
   /**
    * Returns \c true if the value of the figure of merit is invariant under the
    * transormation \f$a_j \mapsto n - a_j\f$ for any \f$j=1,\dots,s\f$, where
    * \f$n\f$ and \f$s\f$ are, respectively, the number of points and the
    * dimension of the lattice and \f$a_j\f$ is the \f$j\f$-th component of the
    * generating vector \f$\boldsymbol a = (a_1, \dots, a_s)\f$.
    */
   virtual bool symmetric() const = 0;

   /**
    * Returns the projection-dependent weights \f$\gamma_{\mathfrak u}\f$.
    */
   virtual const LatCommon::Weights& weights() const = 0;

   virtual std::string name() const = 0;

   virtual Real normType() const = 0;

protected:
   virtual std::ostream& format(std::ostream& os) const = 0;

   friend std::ostream& operator<<(std::ostream&, const FigureOfMerit&);
};

/**
 * Formats \c merit and outputs it on \c os.
 */
inline
std::ostream& operator<<(std::ostream& os, const FigureOfMerit& merit)
{ return merit.format(os); }

}

#endif
