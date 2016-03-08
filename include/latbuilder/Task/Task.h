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

#ifndef LATBUILDER__TASK__TASK_H
#define LATBUILDER__TASK__TASK_H

#include <ostream>
#include <memory>

namespace LatBuilder { namespace Task {

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
