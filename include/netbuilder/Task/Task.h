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

#ifndef NETBUILDER__TASK__TASK_H
#define NETBUILDER__TASK__TASK_H

#include "netbuilder/DigitalNet.h"
#include "netbuilder/Types.h"
#include "netbuilder/NetConstructionTraits.h"

#include <ostream>
#include <memory>

namespace NetBuilder { namespace Task {

/**
 * Base base class for all tasks.
 */
class Task {
public:
   virtual ~Task() {}

   /**
    * Executes the task.
    */
    virtual void execute() = 0;

    /**
     * Outputs the resulting net of the task.
     */ 
    virtual std::string outputNet(OutputFormat outputFormat, OutputStyle outputStyle, unsigned int interlacingFactor) const = 0;

    /**
     * Output information about the task.
     */ 
    virtual std::string format() const = 0;    

    /**
     * Outputs the resulting merit value of the task.
     */ 
    virtual Real outputMeritValue() const = 0;

    /**
     * Resets the task.
     */ 
    virtual void reset() = 0;
};

}}

#endif
