// This file is part of LatNet Builder.
//
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

#ifndef LATBUILDER__COORD_SYM_FIGURE_OF_MERIT_H
#define LATBUILDER__COORD_SYM_FIGURE_OF_MERIT_H

#include "latbuilder/FigureOfMerit.h"
#include "latbuilder/Kernel/Base.h"

#include <memory>
#include <list>

namespace LatBuilder {

/**
 * Base base class for coordinate-uniform figures of merit.
 */
template <class KERNEL>
class CoordUniformFigureOfMerit : public FigureOfMerit
{
public:
   /**
    * Constructor.
    *
    * \param weights    See WeightedFigureOfMerit::WeightedFigureOfMerit for
    *                   details about this parameter.
    * \param kernel     Kernel (\f$\omega\f$ in the reference paper).  See
    *                   the Kernel namespace for examples.
    */
   CoordUniformFigureOfMerit(
         std::unique_ptr<LatticeTester::Weights> weights,
         KERNEL kernel = KERNEL()
         ):
      m_weights(std::move(weights)),
      m_kernel(std::move(kernel))
   {}

   /// \copydoc FigureOfMerit::weights()
   const LatticeTester::Weights& weights() const
   { return *m_weights; }

   /**
    * Returns the coordinate-uniform kernel.
    */
   const KERNEL& kernel() const
   { return m_kernel; }

   /**
    * \copydoc FigureOfMerit::symmetric()
    *
    * Returns \c true if the kernel is symmetric.
    */
   bool symmetric() const
   { return kernel().symmetric(); }

   static constexpr Compress suggestedCompression()
   { return KERNEL::suggestedCompression(); }

   std::string name() const
   { return "CU:" + kernel().name(); }

   static std::string evaluationName()
   { return "coordinate-uniform"; }

   Real normType() const
   { return 2.0; }

protected:
   std::ostream& format(std::ostream& os) const
   {
      return os << "CoordUniformFigureOfMerit("
         << "kernel=" << kernel() << ", "
         << "weights=" << weights()
         << ")";
   }

private:
   std::unique_ptr<LatticeTester::Weights> m_weights;
   KERNEL m_kernel;
};

}

#endif
