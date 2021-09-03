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

#ifndef LATBUILDER__PARSER__KERNEL_H
#define LATBUILDER__PARSER__KERNEL_H

#include "latbuilder/Parser/Common.h"
#include "latbuilder/Kernel/RAlpha.h"
#include "latbuilder/Kernel/PAlpha.h"
#include "latbuilder/Kernel/PAlphaTilde.h"
#include "latbuilder/Kernel/RPLR.h"
#include "latbuilder/Kernel/IAAlpha.h"
#include "latbuilder/Kernel/IB.h"
#include "latbuilder/Kernel/ICAlpha.h"

#include "latbuilder/Interlaced/WeightsInterlacer.h"

namespace LatBuilder { namespace Parser {

/**
 * Exception thrown when trying to parse an invalid kernel.
 */
class BadKernel : public ParserError {
public:
   BadKernel(const std::string& message):
      ParserError("cannot parse coordinate-uniform kernel string: " + message)
   {}
};

/**
 * Parser for kernels for coordinate-uniform figures of merit.
 */
template <LatticeType LR>
struct Kernel
{
   /**
    * Parses a string specifying a kernel for the coordinate-uniform figure of
    * merit, like the \f$\mathcal P_\alpha\f$ and the \f$\mathcal R_\alpha\f$
    * figures of merit.
    *
    * Example strings: <code>P2</code>, <code>P4</code>, <code>P6</code>, <code>R1</code>, <code>R1.5</code>, <code>R2</code>
    *
    * \return A shared pointer to a newly created object or \c nullptr on failure.
    */
   template <typename FUNC, typename... ARGS>
   static void parse(const std::string& str, unsigned int interlacingFactor, std::unique_ptr<LatticeTester::Weights>,  FUNC&& func, ARGS&&... args);
};


template<>
template <typename FUNC, typename... ARGS>
   void Kernel<LatticeType::ORDINARY>::parse(const std::string& str, unsigned int interlacingFactor, std::unique_ptr<LatticeTester::Weights> weights, FUNC&& func, ARGS&&... args)
   {
      try {
             if (str[0] == 'P') {
                auto alpha = boost::lexical_cast<unsigned int>(str.substr(1));
                func(LatBuilder::Kernel::PAlpha(alpha), std::move(weights), std::forward<ARGS>(args)...);
                return;
             }
             else if (str[0] == 'R') {
                auto alpha = boost::lexical_cast<Real>(str.substr(1));
                func(LatBuilder::Kernel::RAlpha(alpha), std::move(weights), std::forward<ARGS>(args)...);
                return;
             }
          }
          catch (boost::bad_lexical_cast&) {}
          throw BadKernel(str);
   }

// template <typename WEIGHTS>
// using WeightsInterlacerA = typename LatBuilder::Interlaced::WeightsInterlacerContainer<LatBuilder::Kernel::IAAlpha>::WeightsInterlacer<WEIGHTS>;

// template <typename WEIGHTS>
// using WeightsInterlacerB = typename LatBuilder::Interlaced::WeightsInterlacerContainer<LatBuilder::Kernel::IB>::WeightsInterlacer<WEIGHTS>;

   
template<>
template <typename FUNC, typename... ARGS>
   void Kernel<LatticeType::POLYNOMIAL>::parse(const std::string& str, unsigned int interlacingFactor, std::unique_ptr<LatticeTester::Weights> weights, FUNC&& func, ARGS&&... args)
   {
      try {
             if (str[0] == 'P') {
                auto alpha = boost::lexical_cast<unsigned int>(str.substr(1));
                func(LatBuilder::Kernel::PAlphaTilde(alpha), std::move(weights), std::forward<ARGS>(args)...);
                return;
             }
             else if (str[0] == 'R') {
                
                func(LatBuilder::Kernel::RPLR(), std::move(weights), std::forward<ARGS>(args)...);
                return;
             }
            else if (str[0] == 'I')
            {
                if (str[1] == 'A')
                {
                    auto alpha = boost::lexical_cast<unsigned int>(str.substr(2));
                    LatBuilder::Kernel::IAAlpha kernel(alpha, interlacingFactor);
                    weights = LatBuilder::WeightsDispatcher::dispatchPtr<LatBuilder::Interlaced::WeightsInterlacer>(std::move(weights), kernel);
                    func(std::move(kernel), std::move(weights), std::forward<ARGS>(args)...);
                    return;
                }
                else if (str[1] == 'B') {
                    LatBuilder::Kernel::IB kernel(interlacingFactor);
                    weights = LatBuilder::WeightsDispatcher::dispatchPtr<LatBuilder::Interlaced::WeightsInterlacer>(std::move(weights), kernel);
                    func(std::move(kernel), std::move(weights), std::forward<ARGS>(args)...);
                    return;
                }
                else if (str[1] == 'C') {
                    auto alpha = boost::lexical_cast<unsigned int>(str.substr(2));
                    LatBuilder::Kernel::ICAlpha kernel(alpha, interlacingFactor);
                    weights = LatBuilder::WeightsDispatcher::dispatchPtr<LatBuilder::Interlaced::WeightsInterlacer>(std::move(weights), kernel);
                    func(std::move(kernel), std::move(weights), std::forward<ARGS>(args)...);
                    return;
                }
            }
          }
          catch (boost::bad_lexical_cast&) {}
          throw BadKernel(str);
   }

/*
extern template struct Kernel<LatticeType::ORDINARY>;
extern template struct Kernel<LatticeType::POLYNOMIAL>;
*/

}}

#endif
