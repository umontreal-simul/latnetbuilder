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

#include "latcommon/Coordinates.h"

using namespace std;

namespace LatCommon
{

bool Coordinates::humanize = true;

ostream& operator<< (ostream& os, const Coordinates& coords)
{
   os << "{";
   Coordinates::const_iterator it = coords.begin();
   if (it != coords.end()) {
      os << Coordinates::asOutput(*it);
      while (++it != coords.end())
         os << "," << Coordinates::asOutput(*it);
   }
   os << "}";
   return os;
}

istream& operator>> (istream& is, Coordinates& coords)
{
   coords.clear();

   string digits = "0123456789";
   string sep = " \t,";

   // check if coordinate set is enclosed in braces
   bool with_braces = false;
   if (is.peek() == '{') {
	  is.get();
	  with_braces = true;
   }

   while (true) {
	  if (with_braces && is.peek() == '}') {
		 is.get();
		 break;
	  }
	  if (digits.find(is.peek()) != string::npos) {
		 // digit found
		 Coordinates::value_type val;
		 is >> val;
		 coords.insert(Coordinates::asInput(val));
		 continue;
	  }
	  if (sep.find(is.peek()) != string::npos) {
		 // discard separator character
		 is.get();
		 continue;
	  }
	  break;
   }

   return is;
}

}
