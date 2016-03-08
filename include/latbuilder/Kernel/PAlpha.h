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

#ifndef LATBUILDER__KERNEL__P_ALPHA_H
#define LATBUILDER__KERNEL__P_ALPHA_H

#include "latbuilder/Kernel/Base.h"
#include "latbuilder/Kernel/FunctorAdaptor.h"
#include "latbuilder/Functor/PAlpha.h"

namespace LatBuilder { namespace Kernel {

class PAlpha : public FunctorAdaptor<Functor::PAlpha> {
public:
   PAlpha(unsigned int alpha):
	  FunctorAdaptor<Functor>(Functor(alpha))
   {}

   unsigned int alpha() const
   { return functor().alpha(); }
};

}}

#endif
