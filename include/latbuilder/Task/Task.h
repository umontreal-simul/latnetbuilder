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

#ifndef LATBUILDER__TASK__TASK_H
#define LATBUILDER__TASK__TASK_H

#include <ostream>
#include <memory>

namespace LatBuilder { namespace Task {

static const char * LatticeTypeStrings[] = { "Ordinary", "Polynomial", "Digital" };
namespace{
    // to suppress unused warning
    void detail(){
        (void) LatticeTypeStrings;
        return;
    }
}

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

protected:
   virtual void format(std::ostream& os) const = 0;
   friend std::ostream& operator<<(std::ostream&, const Task&);
};

/**
 * Formats and outputs \c task to \c os.
 */
inline std::ostream& operator<<(std::ostream& os, const Task& task)
{ task.format(os); return os; }

}}

#endif
