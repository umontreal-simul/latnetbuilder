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

#include "latbuilder/Parser/Weights.h"
#include "latbuilder/Types.h"

#include "latcommon/ProjectionDependentWeights.h"
#include "latcommon/OrderDependentWeights.h"
#include "latcommon/PODWeights.h"
#include "latcommon/ProductWeights.h"

#include "latcommon/Coordinates.h"

namespace LatBuilder { namespace Parser {

std::unique_ptr<LatCommon::Weights>
Weights::parseProjectionDependent(const std::string& arg)
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
      w->setWeight(std::move(proj), s2.first);
   }
   return std::unique_ptr<LatCommon::Weights>(w);
}

std::unique_ptr<LatCommon::Weights>
Weights::parseOrderDependent(const std::string& arg)
{
   auto ka = splitPair<>(arg, ':');
   if (ka.first != "order-dependent") return nullptr;
   auto s = splitPair<Real, std::string>(ka.second, ':');
   auto x = splitCSV<Real>(s.second);
   auto w = new LatCommon::OrderDependentWeights;
   w->setDefaultWeight(s.first);
   for (size_t i = 0; i < x.size(); i++)
      w->setWeightForOrder(i + 1, x[i]);
   return std::unique_ptr<LatCommon::Weights>(w);
}

std::unique_ptr<LatCommon::Weights>
Weights::parseProduct(const std::string& arg)
{
   auto ka = splitPair<>(arg, ':');
   if (ka.first != "product") return nullptr;
   auto s = splitPair<Real, std::string>(ka.second, ':');
   auto x = splitCSV<Real>(s.second);
   auto w = new LatCommon::ProductWeights;
   w->setDefaultWeight(s.first);
   for (size_t i = 0; i < x.size(); i++)
      w->setWeightForCoordinate(i, x[i]);
   return std::unique_ptr<LatCommon::Weights>(w);
}

std::unique_ptr<LatCommon::Weights>
Weights::parsePOD(const std::string& arg)
{
   auto ka = splitPair<>(arg, ':');
   if (ka.first != "POD") return nullptr;
   auto s1 = splitPair<Real, std::string>(ka.second, ':');
   auto s2 = splitPair<std::string, std::string>(s1.second, ':');
   auto s3 = splitCSV<Real>(s2.first);
   auto s4 = splitPair<Real, std::string>(s2.second, ':');
   auto s5 = splitCSV<Real>(s4.second);
   auto w = new LatCommon::PODWeights;
   w->getOrderDependentWeights().setDefaultWeight(s1.first);
   for (size_t i = 0; i < s3.size(); i++)
      w->getOrderDependentWeights().setWeightForOrder(i + 1, s3[i]);
   w->getProductWeights().setDefaultWeight(s4.first);
   for (size_t i = 0; i < s5.size(); i++)
      w->getProductWeights().setWeightForCoordinate(i, s5[i]);
   return std::unique_ptr<LatCommon::Weights>(w);
}

std::unique_ptr<LatCommon::Weights> 
Weights::parse(const std::string& arg)
{
   if (auto p = parseProjectionDependent(arg))
      return p;
   if (auto p = parsePOD(arg))
      return p;
   if (auto p = parseOrderDependent(arg))
      return p;
   if (auto p = parseProduct(arg))
      return p;
   throw BadWeights(arg);
}

}}
