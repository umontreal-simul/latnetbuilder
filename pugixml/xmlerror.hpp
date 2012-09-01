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

#include "pugixml.hpp"
#include <stdexcept>
#include <sstream>
#include <string>

namespace pugi
{

class xml_error : public std::runtime_error {
   public:
	  explicit xml_error (const xml_node& node_, const std::string& what_) :
		 std::runtime_error(make_message(node_, what_)) {}
	  virtual ~xml_error() throw() {}
	  static const std::string make_message(const xml_node& node, const std::string& what_) {
		 std::ostringstream os;
		 os << "xml://";
		 if (node)
			os << node.path() << " (offset " << node.offset_debug() << ")";
		 else
			os << "<unknown path>";
		 os << ": " << what_ << std::ends;
		 return os.str();
	  }
};

}
