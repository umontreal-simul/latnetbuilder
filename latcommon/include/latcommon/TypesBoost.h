// Copyright (c) 2014 David Munger, Pierre L'Ecuyer, Université de Montréal.  
// Copyright (c) 2012 Richard Simard, Pierre L'Ecuyer, Université de Montréal. 
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
// 
// 
// Converted from tcode to Doxygen in 2014.

/**
 * 
 * 
 * Defines \c typedef's for the basic types used in
 * Lattice Builder.
 *
 * Uses vectors and matrices implemented in the library
 * \c ublas from the Boost C++ libraries
 * (see the Boost web page at \\url{http://www.boost.org/} and the
 * \c ublas web page at
 * \\url{http://www.boost.org/doc/libs/1_46_1/libs/numeric/ublas/doc/index.htm}).
 * 
 * 
 * \note Richard: David aimerait remplacer <tt>NScal, NVect</tt>, \c NMat,
 * <tt>RScal, RVect</tt>, \c RMat,
 * <tt>MScal, MVect</tt>, \c MMat,
 * <tt>BScal, BVect</tt>, \c BMat par des noms plus significatifs
 * (il sera plus facile de savoir \`a quoi ils servent).
 * 
 */

#ifndef TYPESBOOST_H
#define TYPESBOOST_H

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>


typedef long                                 MScal;
typedef boost::numeric::ublas::vector<long>  MVect;
typedef boost::numeric::ublas::matrix<long>  MMat;

typedef long                                 BScal;
typedef boost::numeric::ublas::vector<long>  BVect;
typedef boost::numeric::ublas::matrix<long>  BMat;

typedef double                                 NScal;
typedef boost::numeric::ublas::vector<double>  NVect;
typedef boost::numeric::ublas::matrix<double>  NMat;

typedef double                                 RScal;
typedef boost::numeric::ublas::vector<double>  RVect;
typedef boost::numeric::ublas::matrix<double>  RMat;


#endif

