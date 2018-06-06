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

#ifndef LATBUILDER__FUNCTOR__LOW_PASS_H
#define LATBUILDER__FUNCTOR__LOW_PASS_H

namespace LatBuilder { namespace Functor {
/**
 * Low pass filter.
 */
template <typename T>
class LowPass {
public:
   /**
    * Constructor.
    * \param threshold Threshold value.
    */
   LowPass(T threshold = std::numeric_limits<T>::infinity()):
      m_threshold{std::move(threshold)}
   {}
   /**
    * Returns \c true if \c value is below the specified threshold.
    */
   bool operator()(const T& x) const
   { return x < m_threshold; }
   /**
    * Sets the threshold to \c threshold.
    */
   void setThreshold(T threshold)
   { m_threshold = std::move(threshold); }

   const T& threshold() const
   { return m_threshold; }

private:
   T m_threshold;
};

}}
#endif
