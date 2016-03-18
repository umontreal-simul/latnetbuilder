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

#include "latbuilder/Parser/MeritFilter.h"
#include "latbuilder/Parser/LevelWeights.h"
#include "latbuilder/MeritFilter.h"
#include "latbuilder/Norm/Normalizer.h"
#include "latbuilder/Norm/PAlphaSL10.h"
#include "latbuilder/Norm/PAlphaDPW08.h"
#include "latbuilder/Functor/LowPass.h"

#include <boost/lexical_cast.hpp>

namespace LatBuilder { namespace Parser {

namespace {

   template <class NORM>
   void setLevelWeights(
         Norm::Normalizer<LatType::ORDINARY, NORM>&,
         const std::string&,
         const LatBuilder::SizeParam<LatType::ORDINARY>&
         )
   {}

   template <class NORM>
   void setLevelWeights(
         Norm::Normalizer<LatType::EMBEDDED, NORM>& normalizer,
         const std::string& levelWeights,
         const LatBuilder::SizeParam<LatType::EMBEDDED>& sizeParam
         )
   { normalizer.setWeights(LevelWeights::parse(levelWeights, sizeParam)); }

   template <class NORM, LatType LAT>
   std::unique_ptr<BasicMeritFilter<LAT>> createNormalizer(
         unsigned int alpha,
         const LatBuilder::SizeParam<LAT>& sizeParam,
         const LatCommon::Weights& weights,
         Real normType,
         const std::string& levelWeights
         )
   {
      auto normalizer = new LatBuilder::Norm::Normalizer<LAT, NORM>(NORM(alpha, weights, normType));
      setLevelWeights(*normalizer, levelWeights, sizeParam);
      return std::unique_ptr<BasicMeritFilter<LAT>>(normalizer);
   }

   /**
    * Parses the bound PAlphaSL10.
    *
    * Example strings: \c P2-SL10, \c P4-SL10, \c P2-DPW08, \c P4-DPW08
    */
   template <LatType LAT>
   std::unique_ptr<BasicMeritFilter<LAT>> parseNormalizer(
         const std::string& str,
         const LatBuilder::SizeParam<LAT>& sizeParam,
         const LatCommon::Weights& weights,
         Real normType
         )
   {
      const auto args = splitPair(str, ':');
      const auto strSplit = splitPair<>(args.first, '-');
      try {
         if (strSplit.first[0] == 'P') {
            const auto alpha = boost::lexical_cast<unsigned int>(strSplit.first.substr(1));
            if (strSplit.second == "SL10")
               return createNormalizer<LatBuilder::Norm::PAlphaSL10>(alpha, sizeParam, weights, normType, args.second);
            else if (strSplit.second == "DPW08")
               return createNormalizer<LatBuilder::Norm::PAlphaDPW08>(alpha, sizeParam, weights, normType, args.second);
         }
      }
      catch (boost::bad_lexical_cast&) {}
      throw BadFilter("cannot parse norm: " + str);
   }
}

template <LatType LAT>
std::unique_ptr<BasicMeritFilter<LAT>>
MeritFilter::parse(
      const std::string& str,
      const LatBuilder::SizeParam<LAT>& sizeParam,
      const LatCommon::Weights& weights,
      Real normType
      )
{
   const auto x = splitPair(str, ':');
   if (x.first == "norm")
      return parseNormalizer(x.second, sizeParam, weights, normType);
   else if (x.first == "low-pass") {
      auto threshold = boost::lexical_cast<Real>(x.second);
      return std::unique_ptr<BasicMeritFilter<LAT>>(new LatBuilder::MeritFilter<LAT>(Functor::LowPass<Real>(threshold), str));
   }
   throw BadFilter(x.first);
}

template
std::unique_ptr<BasicMeritFilter<LatType::ORDINARY>>
MeritFilter::parse(
      const std::string&,
      const LatBuilder::SizeParam<LatType::ORDINARY>&,
      const LatCommon::Weights&,
      Real);

template
std::unique_ptr<BasicMeritFilter<LatType::EMBEDDED>>
MeritFilter::parse(
      const std::string&,
      const LatBuilder::SizeParam<LatType::EMBEDDED>&,
      const LatCommon::Weights&,
      Real);

}}
