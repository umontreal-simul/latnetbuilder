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

#ifndef LATBUILDER__FIGURE_OF_MERIT_H
#define LATBUILDER__FIGURE_OF_MERIT_H

#include "latbuilder/Types.h"
#include "latbuilder/LatDef.h"

#include "latticetester/Weights.h"

#include <ostream>

namespace LatBuilder
{

/**
 * Abstract base class for figures of merit.
 */
class FigureOfMerit {
public:
   /**
    * Destructor.
    */
   virtual ~FigureOfMerit() {}

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
   virtual const LatticeTester::Weights& weights() const = 0;

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
