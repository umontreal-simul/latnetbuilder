// This file is part of LatNet Builder.
//
// Copyright (C) 2012-2021  The LatNet Builder author's, supervised by Pierre L'Ecuyer, Universite de Montreal.
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

#ifndef LATBUILDER__TASK__MACROS_H
#define LATBUILDER__TASK__MACROS_H

// helper macros for instatiating all task variants

#include "latbuilder/Types.h"
#include "latbuilder/WeightedFigureOfMerit.h"
#include "latbuilder/CoordUniformFigureOfMerit.h"
#include "latbuilder/ProjDepMerit/Spectral.h"
#include "latticetester/NormaBestLat.h"
#include "latbuilder/ProjDepMerit/CoordUniform.h"
#include "latbuilder/Kernel/PAlpha.h"
#include "latbuilder/Kernel/PAlphaTilde.h"
#include "latbuilder/Kernel/RAlpha.h"
#include "latbuilder/Kernel/RPLR.h"
#include "latbuilder/Functor/binary.h"

#define TASK_ADD_ARG_PARAMETERS_LATTICE_ORDINARY(func, ...) \
   func(__VA_ARGS__,LatticeType::ORDINARY, EmbeddingType::UNILEVEL, Compress::NONE, PerLevelOrder::BASIC); \
   func(__VA_ARGS__,LatticeType::ORDINARY, EmbeddingType::UNILEVEL, Compress::SYMMETRIC, PerLevelOrder::BASIC); \
   func(__VA_ARGS__,LatticeType::ORDINARY, EmbeddingType::MULTILEVEL, Compress::NONE, PerLevelOrder::CYCLIC); \
   func(__VA_ARGS__,LatticeType::ORDINARY, EmbeddingType::MULTILEVEL, Compress::SYMMETRIC, PerLevelOrder::CYCLIC)

#define TASK_ADD_ARG_PARAMETERS_LATTICE_POLYNOMIAL(func, ...) \
   func(__VA_ARGS__,LatticeType::POLYNOMIAL, EmbeddingType::UNILEVEL, Compress::NONE, PerLevelOrder::BASIC); \
   func(__VA_ARGS__,LatticeType::POLYNOMIAL, EmbeddingType::MULTILEVEL, Compress::NONE, PerLevelOrder::BASIC); \
   func(__VA_ARGS__,LatticeType::POLYNOMIAL, EmbeddingType::MULTILEVEL, Compress::NONE, PerLevelOrder::CYCLIC)



#define TASK_ADD_ARG_ACCUMULATOR(func, ...) \
   func(__VA_ARGS__, Functor::Sum); \
   func(__VA_ARGS__, Functor::Max)

#define TASK_ADD_ARG_PROJDEP_LATTICE_ORDINARY(func, ...) \
   func(__VA_ARGS__, ProjDepMerit::Spectral<LatticeTester::NormaBestLat<Real>>); \
   func(__VA_ARGS__, ProjDepMerit::CoordUniform<Kernel::PAlpha>); \
   func(__VA_ARGS__, ProjDepMerit::CoordUniform<Kernel::RAlpha>)

#define TASK_ADD_ARG_PROJDEP_LATTICE_POLYNOMIAL(func, ...) \
   func(__VA_ARGS__, ProjDepMerit::CoordUniform<Kernel::PAlphaTilde>); \
   func(__VA_ARGS__, ProjDepMerit::CoordUniform<Kernel::RPLR>)

#define TASK_ADD_ARG_KERNEL_LATTICE_ORDINARY(func, ...) \
   func(__VA_ARGS__, Kernel::PAlpha); \
   func(__VA_ARGS__, Kernel::RAlpha)

#define TASK_ADD_ARG_KERNEL_LATTICE_POLYNOMIAL(func, ...) \
   func(__VA_ARGS__, Kernel::PAlphaTilde); \
   func(__VA_ARGS__, Kernel::RPLR)

#define TASK_INDIRECT(func, ...) \
	func(__VA_ARGS__)

#define TASK_ADD_WEIGHTED_FIGURE(func, z1, z2, x, y, ...) \
   func(z1, z2, __VA_ARGS__, WeightedFigureOfMerit<x, y>)

#define TASK_ADD_COORDSYM_FIGURE(func, z1, z2, x, ...) \
   func(z1, z2, __VA_ARGS__, CoordUniformFigureOfMerit<x>)

#define TASK_FOR_ALL_WEIGHTED_LATTICE_ORDINARY(func, ...) \
   TASK_INDIRECT( \
		   TASK_ADD_ARG_PROJDEP_LATTICE_ORDINARY, \
		   TASK_ADD_ARG_ACCUMULATOR, \
		   TASK_ADD_ARG_PARAMETERS_LATTICE_ORDINARY, \
		   TASK_ADD_WEIGHTED_FIGURE, \
		   func, __VA_ARGS__)

#define TASK_FOR_ALL_WEIGHTED_LATTICE_POLYNOMIAL(func, ...) \
   TASK_INDIRECT( \
         TASK_ADD_ARG_PROJDEP_LATTICE_POLYNOMIAL, \
         TASK_ADD_ARG_ACCUMULATOR, \
         TASK_ADD_ARG_PARAMETERS_LATTICE_POLYNOMIAL, \
         TASK_ADD_WEIGHTED_FIGURE, \
         func, __VA_ARGS__)


#define TASK_FOR_ALL_COORDSYM_LATTICE_ORDINARY(func, ...) \
   TASK_INDIRECT( \
		   TASK_ADD_ARG_KERNEL_LATTICE_ORDINARY, \
		   TASK_ADD_ARG_PARAMETERS_LATTICE_ORDINARY, \
		   TASK_ADD_COORDSYM_FIGURE, \
		   func, __VA_ARGS__)

#define TASK_FOR_ALL_COORDSYM_LATTICE_POLYNOMIAL(func, ...) \
   TASK_INDIRECT( \
         TASK_ADD_ARG_KERNEL_LATTICE_POLYNOMIAL, \
         TASK_ADD_ARG_PARAMETERS_LATTICE_POLYNOMIAL, \
         TASK_ADD_COORDSYM_FIGURE, \
         func, __VA_ARGS__)


#define TASK_FOR_ALL_COORDSYM(func, ...) \
   TASK_FOR_ALL_COORDSYM_LATTICE_ORDINARY(func, __VA_ARGS__); \
   TASK_FOR_ALL_COORDSYM_LATTICE_POLYNOMIAL(func, __VA_ARGS__)

#define TASK_FOR_ALL_WEIGHTED(func, ...) \
   TASK_FOR_ALL_WEIGHTED_LATTICE_ORDINARY(func, __VA_ARGS__); \
   TASK_FOR_ALL_WEIGHTED_LATTICE_POLYNOMIAL(func, __VA_ARGS__); 


#define TASK_FOR_ALL(func, ...) \
   TASK_FOR_ALL_WEIGHTED(func, __VA_ARGS__); \
   TASK_FOR_ALL_COORDSYM(func, __VA_ARGS__); 

#define TASK_BIND_TEMPLATE(Base, TagName, ...) template class Base<TagName##Tag<__VA_ARGS__>>
#define TASK_EXTERN_TEMPLATE(Base, TagName, ...) extern TASK_BIND_TEMPLATE(Base, TagName, __VA_ARGS__)

#define TASK_BIND_TEMPLATE1(Base, TagName, ...) template class Base<__VA_ARGS__>
#define TASK_EXTERN_TEMPLATE1(Base, TagName, ...) extern TASK_BIND_TEMPLATE1(Base, TagName, __VA_ARGS__)

#endif
