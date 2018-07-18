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

#include "latbuilder/Parser/MeritFilter.h"
#include "latbuilder/Parser/LevelWeights.h"
#include "latbuilder/MeritFilter.h"
#include "latbuilder/Norm/Normalizer.h"
#include "latbuilder/Norm/PAlphaSL10.h"
#include "latbuilder/Norm/PAlphaDPW08.h"
#include "latbuilder/Norm/PAlphaPLR.h"
#include "latbuilder/Norm/IAAlphaG15.h"
#include "latbuilder/Norm/IBG15.h"
#include "latbuilder/Functor/LowPass.h"

#include <boost/lexical_cast.hpp>

namespace LatBuilder { namespace Parser {

namespace {

   template <LatticeType LR, class NORM>
   void setLevelWeights(
         Norm::Normalizer<LR, EmbeddingType::UNILEVEL, NORM>&,
         const std::string&,
         const LatBuilder::SizeParam<LR, EmbeddingType::UNILEVEL>&
         )
   {}

   template <LatticeType LR, class NORM>
   void setLevelWeights(
         Norm::Normalizer<LR, EmbeddingType::MULTILEVEL, NORM>& normalizer,
         const std::string& levelWeights,
         const LatBuilder::SizeParam<LR, EmbeddingType::MULTILEVEL>& sizeParam
         )
   { normalizer.setWeights(LevelWeights<LR>::parse(levelWeights, sizeParam)); }

   template <LatticeType LR, class NORM, EmbeddingType ET>
   std::unique_ptr<BasicMeritFilter<LR, ET>> createNormalizer(
         unsigned int alpha,
         const LatBuilder::SizeParam<LR, ET>& sizeParam,
         const LatticeTester::Weights& weights,
         Real normType,
         const std::string& levelWeights
         )
   {
      auto normalizer = new LatBuilder::Norm::Normalizer<LR, ET, NORM>(NORM(alpha, weights, normType));
      setLevelWeights(*normalizer, levelWeights, sizeParam);
      return std::unique_ptr<BasicMeritFilter<LR, ET>>(normalizer);
   }

   template <LatticeType LR, EmbeddingType ET> void correctLevelWeights(
         const LatBuilder::SizeParam<LR, ET>& sizeParam, 
         const std::string& combiner,
         std::string& levelWeights);

   template <LatticeType LR>
   void correctLevelWeights(
         const LatBuilder::SizeParam<LR, EmbeddingType::UNILEVEL>& sizeParam, 
         const std::string& combiner,
         std::string& levelWeights)
      {}

   template <LatticeType LR>
   void correctLevelWeights(
         const LatBuilder::SizeParam<LR, EmbeddingType::MULTILEVEL>& sizeParam, 
         const std::string& combiner,
         std::string& levelWeights)
      {
            const auto strCombinerSplit = splitPair<>(combiner, ':');
            if (strCombinerSplit.first == "level") {
                  if (strCombinerSplit.second == "max"){
                        levelWeights = "even:" + std::to_string(sizeParam.maxLevel());
                  }
                  else {
                        try {
                              levelWeights = "even:" + std::to_string(boost::lexical_cast<Level>(strCombinerSplit.second)) + "," + std::to_string(boost::lexical_cast<Level>(strCombinerSplit.second));
                        }
                        catch (boost::bad_lexical_cast&) {}
                  }
            }
      }

   /**
    * Parses the bound PAlphaSL10.
    *
    * Example strings: \c P2-SL10, \c P4-SL10, \c P2-DPW08, \c P4-DPW08
    */
   template <LatticeType LR, EmbeddingType ET>
   std::unique_ptr<BasicMeritFilter<LR, ET>> parseNormalizer(
         const std::string& str,
         const LatBuilder::SizeParam<LR, ET>& sizeParam,
         const LatticeTester::Weights& weights,
         Real normType,
         std::string combiner
         )
   {
      auto args = splitPair(str, ':');
      const auto strSplit = splitPair<>(args.first, '-');

      correctLevelWeights(sizeParam, combiner, args.second);

      try {
      if (strSplit.first[0] == 'P') {
            const auto alpha = boost::lexical_cast<unsigned int>(strSplit.first.substr(1));
            if (strSplit.second == "SL10")
            return createNormalizer<LR, LatBuilder::Norm::PAlphaSL10, ET>(alpha, sizeParam, weights, normType, args.second);
            else if (strSplit.second == "DPW08")
            return createNormalizer<LR, LatBuilder::Norm::PAlphaDPW08, ET>(alpha, sizeParam, weights, normType, args.second);
            else if (strSplit.second == "PLR")
            return createNormalizer<LR, LatBuilder::Norm::PAlphaPLR, ET>(alpha, sizeParam, weights, normType, args.second);
      }
      if (strSplit.first[0] == 'I')
      {
            if (strSplit.first[1] == 'A')
            {
                  const auto alpha = boost::lexical_cast<unsigned int>(strSplit.first.substr(2));
                  return createNormalizer<LR, LatBuilder::Norm::IAAlphaG15, ET>(alpha, sizeParam, weights, normType, args.second);
            }
            if (strSplit.first[1] == 'B')
            {
                  return createNormalizer<LR, LatBuilder::Norm::IBG15, ET>(weights.interlacingFactor(), sizeParam, weights, normType, args.second);
            }
      }
      }
      catch (boost::bad_lexical_cast&) {}
      throw BadFilter("cannot parse norm: " + str);
      
   }
}

template <LatticeType LR, EmbeddingType ET>
std::unique_ptr<BasicMeritFilter<LR, ET>>
MeritFilter<LR,ET>::parse(
      const std::string& str,
      const LatBuilder::SizeParam<LR, ET>& sizeParam,
      const LatticeTester::Weights& weights,
      Real normType,
      std::string combiner
      )
{
   const auto x = splitPair(str, ':');
   if (x.first == "norm")
      return parseNormalizer(x.second, sizeParam, weights, normType, combiner);
   else if (x.first == "low-pass") {
      auto threshold = boost::lexical_cast<Real>(x.second);
      return std::unique_ptr<BasicMeritFilter<LR, ET>>(new LatBuilder::MeritFilter<LR, ET>(Functor::LowPass<Real>(threshold), str));
   }
   throw BadFilter(x.first);
}

template struct LatBuilder::Parser::MeritFilter <LatticeType::ORDINARY, EmbeddingType::UNILEVEL> ;
template struct LatBuilder::Parser::MeritFilter <LatticeType::ORDINARY, EmbeddingType::MULTILEVEL> ;
template struct LatBuilder::Parser::MeritFilter <LatticeType::POLYNOMIAL, EmbeddingType::UNILEVEL> ;
template struct LatBuilder::Parser::MeritFilter <LatticeType::POLYNOMIAL, EmbeddingType::MULTILEVEL> ;


/*
template
std::unique_ptr<BasicMeritFilter<EmbeddingType::UNILEVEL>>
MeritFilter::parse(
      const std::string&,
      const LatBuilder::SizeParam<EmbeddingType::UNILEVEL>&,
      const LatticeTester::Weights&,
      Real);

template
std::unique_ptr<BasicMeritFilter<EmbeddingType::MULTILEVEL>>
MeritFilter::parse(
      const std::string&,
      const LatBuilder::SizeParam<EmbeddingType::MULTILEVEL>&,
      const LatticeTester::Weights&,
      Real);
*/
}}
