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

#include "latbuilder/Parser/Weights.h"
#include "latbuilder/Types.h"

#include "latcommon/ProjectionDependentWeights.h"
#include "latcommon/OrderDependentWeights.h"
#include "latcommon/PODWeights.h"
#include "latcommon/ProductWeights.h"

#include "latcommon/Coordinates.h"

#include <cmath>

namespace LatBuilder { namespace Parser {

std::unique_ptr<LatCommon::Weights>
Weights::parseProjectionDependent(const std::string& arg, Real powerScale)
{
   auto ka = splitPair<>(arg, ':');
   if (ka.first != "projection-dependent") return nullptr;
   auto w = new LatCommon::ProjectionDependentWeights;
   std::string rest = ka.second;
   while (!rest.empty()) {
      auto s1 = splitPair<>(rest, ':');
      auto s2 = splitPair<LatCommon::Weight, std::string>(s1.second, ':');
      auto p = splitCSV<Modulus>(s1.first);
      for (auto& pi : p) pi--;
      rest = s2.second;
      LatCommon::Coordinates proj(p.begin(), p.end());
      w->setWeight(std::move(proj), std::pow(s2.first, powerScale));
   }
   return std::unique_ptr<LatCommon::Weights>(w);
}

std::unique_ptr<LatCommon::Weights>
Weights::parseOrderDependent(const std::string& arg, Real powerScale)
{
   auto ka = splitPair<>(arg, ':');
   if (ka.first != "order-dependent") return nullptr;
   auto s = splitPair<Real, std::string>(ka.second, ':');
   auto x = splitCSV<Real>(s.second);
   auto w = new LatCommon::OrderDependentWeights;
   w->setDefaultWeight(std::pow(s.first, powerScale));
   for (size_t i = 0; i < x.size(); i++)
      w->setWeightForOrder(i + 1, std::pow(x[i], powerScale));
   return std::unique_ptr<LatCommon::Weights>(w);
}

std::unique_ptr<LatCommon::Weights>
Weights::parseProduct(const std::string& arg, Real powerScale)
{
   auto ka = splitPair<>(arg, ':');
   if (ka.first != "product") return nullptr;
   auto s = splitPair<Real, std::string>(ka.second, ':');
   auto x = splitCSV<Real>(s.second);
   auto w = new LatCommon::ProductWeights;
   w->setDefaultWeight(std::pow(s.first, powerScale));
   for (size_t i = 0; i < x.size(); i++)
      w->setWeightForCoordinate(i, std::pow(x[i], powerScale));
   return std::unique_ptr<LatCommon::Weights>(w);
}

std::unique_ptr<LatCommon::Weights>
Weights::parsePOD(const std::string& arg, Real powerScale)
{
   auto ka = splitPair<>(arg, ':');
   if (ka.first != "POD") return nullptr;
   auto s1 = splitPair<Real, std::string>(ka.second, ':');
   auto s2 = splitPair<std::string, std::string>(s1.second, ':');
   auto s3 = splitCSV<Real>(s2.first);
   auto s4 = splitPair<Real, std::string>(s2.second, ':');
   auto s5 = splitCSV<Real>(s4.second);
   auto w = new LatCommon::PODWeights;
   w->getOrderDependentWeights().setDefaultWeight(std::pow(s1.first, powerScale));
   for (size_t i = 0; i < s3.size(); i++)
      w->getOrderDependentWeights().setWeightForOrder(i + 1, std::pow(s3[i], powerScale));
   w->getProductWeights().setDefaultWeight(std::pow(s4.first, powerScale));
   for (size_t i = 0; i < s5.size(); i++)
      w->getProductWeights().setWeightForCoordinate(i, std::pow(s5[i], powerScale));
   return std::unique_ptr<LatCommon::Weights>(w);
}

std::unique_ptr<LatCommon::Weights> 
Weights::parse(const std::string& arg, Real powerScale)
{
   if (auto p = parseProjectionDependent(arg, powerScale))
      return p;
   if (auto p = parsePOD(arg, powerScale))
      return p;
   if (auto p = parseOrderDependent(arg, powerScale))
      return p;
   if (auto p = parseProduct(arg, powerScale))
      return p;
   throw BadWeights(arg);
}

}}
