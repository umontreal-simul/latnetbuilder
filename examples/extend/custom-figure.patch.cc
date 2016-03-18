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

diff --git a/include/latbuilder/Functor/MyFigure.h b/include/latbuilder/Functor/MyFigure.h
new file mode 100644
index 0000000..6a16f59
--- /dev/null
+++ b/include/latbuilder/Functor/MyFigure.h
@@ -0,0 +1,35 @@
#ifndef LATBUILDER__FUNCTOR__MYFIGURE_H
#define LATBUILDER__FUNCTOR__MYFIGURE_H

//! [functor def]
#include "latbuilder/Types.h"
#include <sstream>

namespace LatBuilder { namespace Functor {

class MyFigure {
public:
   typedef Real value_type;
   typedef Real result_type;

   bool symmetric() const
   { return true; }

   static constexpr Compress suggestedCompression()
   { return Compress::SYMMETRIC; }

   result_type operator()(const value_type& x, Modulus n = 0) const
   { return x * (1.0 - x); /* change this to suit your needs */ }

   std::string name() const
   { return "my-figure"; }
};

inline
std::ostream& operator<<(std::ostream& os, const MyFigure& functor)
{ return os << functor.name(); }

}}
//! [functor def]

#endif
diff --git a/include/latbuilder/Kernel/MyFigure.h b/include/latbuilder/Kernel/MyFigure.h
new file mode 100644
index 0000000..e69d2ad
--- /dev/null
+++ b/include/latbuilder/Kernel/MyFigure.h
@@ -0,0 +1,20 @@
#ifndef LATBUILDER__KERNEL__MYFIGURE_H
#define LATBUILDER__KERNEL__MYFIGURE_H

//! [kernel def]
#include "latbuilder/Kernel/FunctorAdaptor.h"
#include "latbuilder/Functor/MyFigure.h"

namespace LatBuilder { namespace Kernel {

class MyFigure : public FunctorAdaptor<Functor::MyFigure> {
public:
   MyFigure():
	  FunctorAdaptor<Functor>(Functor())
   {}
};

}}
//! [kernel def]

#endif
diff --git a/include/latbuilder/Parser/Kernel.h b/include/latbuilder/Parser/Kernel.h
index 1a3005c..3ef140a 100644
--- a/include/latbuilder/Parser/Kernel.h
+++ b/include/latbuilder/Parser/Kernel.h
@@ -21,6 +21,9 @@
 #include "latbuilder/Parser/Common.h"
 #include "latbuilder/Kernel/RAlpha.h"
 #include "latbuilder/Kernel/PAlpha.h"
//! [parser header]
#include "latbuilder/Kernel/MyFigure.h"
//! [parser header]
 
 namespace LatBuilder { namespace Parser {
 
@@ -61,6 +64,12 @@ struct Kernel {
             func(LatBuilder::Kernel::RAlpha(alpha), std::forward<ARGS>(args)...);
             return;
          }
         //! [parser if]
         else if (str == "my-figure") {
            func(LatBuilder::Kernel::MyFigure(), std::forward<ARGS>(args)...);
            return;
         }
         //! [parser if]
       }
       catch (boost::bad_lexical_cast&) {}
       throw BadKernel(str);
