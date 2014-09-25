// Copyright (c) 2012, 2013 David Munger, Pierre L'Ecuyer, Université de Montréal.
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

#ifndef LATBUILDER__COORD_SYM_FIGURE_OF_MERIT_H
#define LATBUILDER__COORD_SYM_FIGURE_OF_MERIT_H

#include "latbuilder/FigureOfMerit.h"
#include "latbuilder/Kernel/Base.h"

#include <memory>
#include <list>

namespace LatBuilder {

/**
 * Base base class for coordinate-symmetric figures of merit.
 */
template <class KERNEL>
class CoordSymFigureOfMerit : public FigureOfMerit
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
   CoordSymFigureOfMerit(
         std::unique_ptr<LatCommon::Weights> weights,
         KERNEL kernel = KERNEL()
         ):
      m_weights(std::move(weights)),
      m_kernel(std::move(kernel))
   {}

   /// \copydoc FigureOfMerit::weights()
   const LatCommon::Weights& weights() const
   { return *m_weights; }

   /**
    * Returns the coordinate-symmetric kernel.
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
   { return "CS:" + kernel().name(); }

   Real normType() const
   { return 2.0; }

protected:
   std::ostream& format(std::ostream& os) const
   {
      return os << "CoordSymFigureOfMerit("
         << "kernel=" << kernel() << ", "
         << "weights=" << weights()
         << ")";
   }

private:
   std::unique_ptr<LatCommon::Weights> m_weights;
   KERNEL m_kernel;
};

}

#endif
