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

diff --git a/include/latbuilder/Parser/Weights.h b/include/latbuilder/Parser/Weights.h
index eb2964e..9991c13 100644
--- a/include/latbuilder/Parser/Weights.h
+++ b/include/latbuilder/Parser/Weights.h
@@ -97,6 +97,11 @@ struct Weights {
    static std::unique_ptr<LatticeTester::Weights>
    parsePOD(const std::string& arg, Real powerScale);
 
   //! [parser decl]
   static std::unique_ptr<LatticeTester::Weights>
   parseMyWeights(const std::string& arg, Real powerScale);
   //! [parser decl]

    /**
     * Parses a string specifying weights.
     *
diff --git a/src/Parser/Weights.cc b/src/Parser/Weights.cc
index 0c07669..8555f08 100644
--- a/src/Parser/Weights.cc
+++ b/src/Parser/Weights.cc
@@ -22,6 +22,9 @@
 #include "latticetester/OrderDependentWeights.h"
 #include "latticetester/PODWeights.h"
 #include "latticetester/ProductWeights.h"
// [parser header]
#include "latticetester/MyWeights.h"
// [parser header]
 
 #include "latticetester/Coordinates.h"
 
@@ -96,6 +99,17 @@ Weights::parsePOD(const std::string& arg, Real powerScale)
    return std::unique_ptr<LatticeTester::Weights>(w);
 }
 
//! [parser impl]
std::unique_ptr<LatticeTester::Weights>
Weights::parseMyWeights(const std::string& arg, Real powerScale)
{
   auto ka = splitPair<std::string, Real>(arg, ':');
   if (ka.first != "my-weights") return nullptr;
   auto w = new LatticeTester::MyWeights(ka.second);
   return std::unique_ptr<LatticeTester::Weights>(w);
}
//! [parser impl]

 std::unique_ptr<LatticeTester::Weights> 
 Weights::parse(const std::string& arg, Real powerScale)
 {
@@ -107,6 +121,10 @@ Weights::parse(const std::string& arg, Real powerScale)
       return p;
    if (auto p = parseProduct(arg, powerScale))
       return p;
   //! [parser if]
   if (auto p = parseMyWeights(arg, powerScale))
      return p;
   //! [parser if]
    throw BadWeights(arg);
 }
 
diff --git a/latticetester/include/latticetester/MyWeights.h b/latticetester/include/latticetester/MyWeights.h
new file mode 100644
index 0000000..e0e0a4d
--- /dev/null
+++ b/latticetester/include/latticetester/MyWeights.h
@@ -0,0 +1,25 @@
#ifndef LATTICETESTER__MY_WEIGHTS_H
#define LATTICETESTER__MY_WEIGHTS_H

//! [weights def]
#include "latticetester/ProductWeights.h"

namespace LatticeTester {

class MyWeights : public ProductWeights {
public:
   explicit MyWeights(Weight parameter=1.0) : m_parameter(parameter) {}
   virtual ~MyWeights() {} 
   virtual Weight getWeightForCoordinate(Coordinates::size_type coordinate) const
   { return std::pow(coordinate + 1, -m_parameter); }

protected:
   virtual void format(std::ostream& os) const
   { os << "MyWeights(" << m_parameter << ")"; }
   Weight m_parameter;
};

}
//! [weights def]

#endif
