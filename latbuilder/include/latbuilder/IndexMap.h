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

#ifndef LATBUILDER__INDEX_MAP_H
#define LATBUILDER__INDEX_MAP_H

#include <boost/numeric/ublas/detail/iterator.hpp>
#include <boost/numeric/ublas/exception.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>

namespace LatBuilder {

/**
 * Permutation of vector indices.
 *
 * This class is meant to permutations of Boost uBLAS vector object.
 * The simplest way to use this class is through the permuteVector() function.
 *
 * Credits: this class is an adaptation of Boost's indirect array class.
 *
 * \example IndexMap.cc
 */
template<class M>
class IndexMap {
    typedef IndexMap<M> self_type;
public:
    typedef M mapper_type;
    typedef const M const_mapper_type;
    typedef typename M::size_type size_type;
    typedef typename M::size_type value_type;
    typedef typename M::size_type difference_type;
    typedef const value_type const_reference;
    typedef value_type reference;

    // Construction and destruction
    BOOST_UBLAS_INLINE
    IndexMap ():
        mapper_ () {}
    explicit BOOST_UBLAS_INLINE
    IndexMap (mapper_type mapper):
        mapper_ (std::move(mapper)) {}

    BOOST_UBLAS_INLINE
    size_type size () const {
        return mapper_.size ();
    }
    BOOST_UBLAS_INLINE
    const_mapper_type& mapper () const {
        return mapper_;
    }
    BOOST_UBLAS_INLINE
    mapper_type& mapper () {
        return mapper_;
    }

    // Random Access Container
    BOOST_UBLAS_INLINE
    size_type max_size () const {
        return mapper_.size ();
    }
    
    BOOST_UBLAS_INLINE
    bool empty () const {
        return mapper_.size () == 0;
    }
        
    // Element access
    BOOST_UBLAS_INLINE
    size_type operator () (size_type i) const {
        BOOST_UBLAS_CHECK (i < size (), boost::numeric::ublas::bad_index ());
        return mapper_ (i);
    }
    BOOST_UBLAS_INLINE
    size_type operator () (size_type i) {
        BOOST_UBLAS_CHECK (i < size (), boost::numeric::ublas::bad_index ());
        return mapper_ (i);
    }

    BOOST_UBLAS_INLINE
    size_type operator [] (size_type i) const {
        return (*this) (i);
    }
    BOOST_UBLAS_INLINE
    size_type operator [] (size_type i) {
        return (*this) (i);
    }

    // Comparison
    template<class OM>
    BOOST_UBLAS_INLINE
    bool operator == (const IndexMap<OM> &ia) const {
        if (size () != ia.size ())
            return false;
        for (size_type i = 0; i < BOOST_UBLAS_SAME (size (), ia.size ()); ++ i)
            if (mapper_ (i) != ia.mapper_ (i))
                return false;
        return true;
    }
    template<class OM>
    BOOST_UBLAS_INLINE
    bool operator != (const IndexMap<OM> &ia) const {
        return not (*this == ia);
    }

public:
    typedef boost::numeric::ublas::indexed_const_iterator<IndexMap, std::random_access_iterator_tag> const_iterator;

    BOOST_UBLAS_INLINE
    const_iterator begin () const {
        return const_iterator (*this, 0);
    }
    BOOST_UBLAS_INLINE
    const_iterator end () const {
        return const_iterator (*this, size ());
    }

    // Reverse iterator
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    BOOST_UBLAS_INLINE
    const_reverse_iterator rbegin () const {
        return const_reverse_iterator (end ());
    }
    BOOST_UBLAS_INLINE
    const_reverse_iterator rend () const {
        return const_reverse_iterator (begin ());
    }
    BOOST_UBLAS_INLINE
    IndexMap preprocess (size_type size) const {
        return *this;
    }

private:
    mapper_type mapper_;
};

/**
 * Returns a vector proxy whose elements are a permutation of another vector.
 *
 * \param vec       Original vector.
 * \param mapper    Object that defines a permutation of vector indices through
 *                  the definition of its member \c operator().
 *
 * To use this function, first define a mapper class, say \c MyMapper, that
 * defines the type \c size_type, that implements \c size() whose return value
 * is number of elements in the permutation, and that implements \c operator()
 * that maps an input index (the operator argument) to an output index, e.g.:
 * \code
 * class MyMapper {
 * public:
 *    typedef unsigned size_type;
 *    size_type size() const { return ...; }
 *    size_type operator() (size_type i) const { return ...; }
 * };
 * \endcode
 * Next, simply instantiate a Boost uBLAS vector \c vec and obtain a
 * permutation \c pvec:
 * \code
 * boost::numeric::ublas::vector<unsigned> vec(10);
 * auto pvec = LatBuilder::permuteVector(vec, MyMapper());
 * \endcode
 *
 * \example IndexMap.cc
 */
template <typename VEC, typename MAP>
boost::numeric::ublas::vector_indirect<VEC,IndexMap<MAP>>
permuteVector(VEC& vec, MAP mapper) {
   typedef IndexMap<MAP> IMap;
   return boost::numeric::ublas::vector_indirect<VEC,IMap>(vec, IMap(std::move(mapper)));
}

}

#endif
