// This file is part of LatticeTester.
//
// LatticeTester
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

#include "netbuilder/JoeKuo.h"
#include "netbuilder/Util.h"
#include <cmath>

#include <string>
#include <fstream>
#include <boost/algorithm/string.hpp>

using namespace std;

namespace NetBuilder { namespace JoeKuo {

//===========================================================================

LatticeTester::Weight Weights::getWeight (const LatticeTester::Coordinates& projection) const
{
   if (projection.size()==2)
   {
         return std::pow((LatticeTester::Weight).9999, *projection.rbegin());
   }
   // fall back to zero
   return 0.0;
}

//===========================================================================

void Weights::format(ostream& os) const
{
   using LatticeTester::operator<<;
   os << "Weights()";
}

//===========================================================================

const char* ws = " \t\n\r\f\v";

// trim from end (right)
inline std::string& rtrim(std::string& s, const char* t = ws)
{
      s.erase(s.find_last_not_of(t) + 1);
      return s;
}

// trim from beginning (left)
inline std::string& ltrim(std::string& s, const char* t = ws)
{
      s.erase(0, s.find_first_not_of(t));
      return s;
}

// trim from both ends (left & right)
inline std::string& trim(std::string& s, const char* t = ws)
{
      return ltrim(rtrim(s, t), t);
}    

std::vector<std::vector<uInteger>> readJoeKuoDirectionNumbers(Dimension dimension)
{
      assert(dimension >= 1 && dimension <= 21201);
      std::ifstream file("../share/latnetbuilder/data/JoeKuoSobolNets.csv");
      std::vector<std::vector<uInteger>> res(dimension);
      std::string sent;

      do
      {
      getline(file,sent);
      trim(sent);
      }
      while (sent != "###");

      getline(file,sent);

      for(unsigned int i = 1; i <= dimension; ++i)
      {
            if(getline(file,sent))
            {
                  std::vector<std::string> fields;
                  boost::split( fields, sent, boost::is_any_of( ";" ) );
                  for( const auto& token : fields)
                  {
                        res[i-1].push_back(std::stol(token));
                  }
            }
            else
            {
                  break;
            }
      }
      return res;
}

std::vector<DirectionNumbers> getJoeKuoDirectionNumbers(Dimension dimension)
{
      std::vector<std::vector<uInteger>> tmp = readJoeKuoDirectionNumbers(dimension);
      std::vector<DirectionNumbers> genVals(dimension);
      for(unsigned int j = 0; j < dimension; ++j)
      {
            genVals[j] = DirectionNumbers(j,tmp[j]);
      }
      return genVals;
}

DigitalNetConstruction<NetConstruction::SOBOL> createJoeKuoSobolNet(Dimension dimension, MatrixSize size)
{
      auto genVals = getJoeKuoDirectionNumbers(dimension);
      return DigitalNetConstruction<NetConstruction::SOBOL>(dimension, size, std::move(genVals));
}

std::unique_ptr<DigitalNetConstruction<NetConstruction::SOBOL>> createPtrToJoeKuoSobolNet(Dimension dimension, MatrixSize size)
{
      auto genVals = getJoeKuoDirectionNumbers(dimension);
      return std::make_unique<DigitalNetConstruction<NetConstruction::SOBOL>>(dimension, size, std::move(genVals));
}

}} // namespace
