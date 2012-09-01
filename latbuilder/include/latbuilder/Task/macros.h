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

#ifndef LATBUILDER__TASK__MACROS_H
#define LATBUILDER__TASK__MACROS_H

// helper macros for instatiating all task variants

#include "latbuilder/Types.h"
#include "latbuilder/WeightedFigureOfMerit.h"
#include "latbuilder/CoordSymFigureOfMerit.h"
#include "latbuilder/ProjDepMerit/Spectral.h"
#include "latcommon/NormaBestLat.h"
#include "latbuilder/ProjDepMerit/CoordSym.h"
#include "latbuilder/Kernel/PAlpha.h"
#include "latbuilder/Kernel/RAlpha.h"
#include "latbuilder/Functor/binary.h"

#define TASK_ADD_ARG_LATTYPE(func, ...) \
   func(__VA_ARGS__, LatType::ORDINARY); \
   func(__VA_ARGS__, LatType::EMBEDDED)

#define TASK_ADD_ARG_COMPRESS(func, ...) \
   func(__VA_ARGS__, Compress::NONE); \
   func(__VA_ARGS__, Compress::SYMMETRIC)

#define TASK_ADD_ARG_ACCUMULATOR(func, ...) \
   func(__VA_ARGS__, Functor::Sum); \
   func(__VA_ARGS__, Functor::Max)

#define TASK_ADD_ARG_PROJDEP(func, ...) \
   func(__VA_ARGS__, ProjDepMerit::Spectral<LatCommon::NormaBestLat>); \
   func(__VA_ARGS__, ProjDepMerit::CoordSym<Kernel::PAlpha>); \
   func(__VA_ARGS__, ProjDepMerit::CoordSym<Kernel::RAlpha>)

#define TASK_ADD_ARG_KERNEL(func, ...) \
   func(__VA_ARGS__, Kernel::PAlpha); \
   func(__VA_ARGS__, Kernel::RAlpha)

#define TASK_INDIRECT(func, ...) \
	func(__VA_ARGS__)

#define TASK_ADD_WEIGHTED_FIGURE(func, z1, z2, x, y, ...) \
   func(z1, z2, __VA_ARGS__, WeightedFigureOfMerit<x, y>)

#define TASK_ADD_COORDSYM_FIGURE(func, z1, z2, x, ...) \
   func(z1, z2, __VA_ARGS__, CoordSymFigureOfMerit<x>)

#define TASK_FOR_ALL_WEIGHTED(func, ...) \
   TASK_INDIRECT( \
		   TASK_ADD_ARG_PROJDEP, \
		   TASK_ADD_ARG_ACCUMULATOR, \
		   TASK_ADD_ARG_LATTYPE, \
		   TASK_ADD_ARG_COMPRESS, \
		   TASK_ADD_WEIGHTED_FIGURE, \
		   func, __VA_ARGS__)

#define TASK_FOR_ALL_COORDSYM(func, ...) \
   TASK_INDIRECT( \
		   TASK_ADD_ARG_KERNEL, \
		   TASK_ADD_ARG_LATTYPE, \
		   TASK_ADD_ARG_COMPRESS, \
		   TASK_ADD_COORDSYM_FIGURE, \
		   func, __VA_ARGS__)

#define TASK_FOR_ALL(func, ...) \
   TASK_FOR_ALL_WEIGHTED(func, __VA_ARGS__); \
   TASK_FOR_ALL_COORDSYM(func, __VA_ARGS__)

#define TASK_BIND_TEMPLATE(Base, TagName, ...) template class Base<TagName##Tag<__VA_ARGS__>>
#define TASK_EXTERN_TEMPLATE(Base, TagName, ...) extern TASK_BIND_TEMPLATE(Base, TagName, __VA_ARGS__)

#define TASK_BIND_TEMPLATE1(Base, TagName, ...) template class Base<__VA_ARGS__>
#define TASK_EXTERN_TEMPLATE1(Base, TagName, ...) extern TASK_BIND_TEMPLATE1(Base, TagName, __VA_ARGS__)

#endif
