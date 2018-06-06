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

#ifndef NETBUILDER__JOE_KUO_WEIGHTS_H
#define NETBUILDER__JOE_KUO_WEIGHTS_H

#include "latticetester/Weights.h"
#include <map>
#include <vector>


namespace NetBuilder {

/**
 * Weights used by Joe and Kuo. Ref: 
 * S. Joe and F. Y. Kuo, <EM> Constructing Sobol sequences with better two-dimensional projections </EM>, 
 * SIAM J. Sci. Comput. <B> 30 </B>, 2635-2654 (2008). TODO
 */ 
class JoeKuoWeights : public LatticeTester::Weights {

public:

   /**
    * Destructor.
    */
   virtual ~JoeKuoWeights()  {} 

   virtual std::string name() const { return "Joe-Kuo weights"; }

   /**
    * Copy constructor.
    */
   JoeKuoWeights (const JoeKuoWeights &) = default;

   /**
    * Returns the weight of the projection specified by \c projection.
    */
   virtual LatticeTester::Weight getWeight (const LatticeTester::Coordinates & projection) const;

protected:
   /// \copydoc LatticeTester::Weights::format()
   virtual void format(std::ostream& os) const;
};

}
#endif
