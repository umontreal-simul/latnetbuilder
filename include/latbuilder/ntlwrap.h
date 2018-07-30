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

#ifndef __NTLWRAP_H__
#define __NTLWRAP_H__

#include <NTL/vector.h>
#include <NTL/matrix.h>
#include <NTL/GF2E.h>



NTL_CLIENT

/**
 * The two floowing two classes are copies from NTL::Vec<T> and NTL::Mat<T>, but
 * they contain additional member functions having same names to the ones 
 * used in boost library.
 * This name conversion is meant to have the samee function names in boost and NTL 
 */

namespace NTL
{


   template <typename T> class vector : public Vec<T>{
         public:

      typedef long size_type;

      /**
      * Constructor.
      */
      vector<T>() {};
      vector<T>(size_type size) : Vec<T>(INIT_SIZE, size) {};
      vector<T>(const Vec<T> &v) : Vec<T>(v) {};
      ~vector () {};

      /**
      * Set the vector lenght to size
      * new objects are initialized using the default contructor for T
      * a copy from NTL::Vec<T>::SetLength
      */
      void resize(size_type size) { this->SetLength(size); }

      /**
      * release space and set to length 0
      * a copy from NTL::Vec<T>::kill
      */
      void clear() { this->kill(); }

      /**
      * a copy from NTL::Vec<T>::length
      */
      size_type size() const { return this->length(); }

      /**
      * a copy from NTL::Vec<T>::MaxLength
      */
      size_type max_size() const { return this->MaxLength(); }

      /**
      * a copy from NTL::Vec<T>::append
      */
      void push_back(const T& element) { this->append(element); }

      /**
      * return the last element, this one is for STL compatibility
      */
      T back() { return (*(this->end()-1)); }
      /**
      * check if a vector is empty
      */
      bool empty() const { return size() == 0; }

      /**
      * change the indexation reference for () operator to start from 0
      * in NTL::Vec<T> the () operator starts from 1 wich is not compatible with boost 
      */
      const T& operator()(size_type i) const { return (*this)[i]; }
      T&  operator()(size_type i) { return (*this)[i]; }

   };





   template <typename T> class matrix : public Mat<T>{

      public:

      typedef long size_type;

      /**
      * Constructor.
      */
      matrix<T>() {}
      matrix<T>(const Mat<T>& a) : Mat<T>(a) {}
      matrix<T>(size_type size1, size_type size2) : Mat<T>(INIT_SIZE, size1, size2) {}

      /**
      * Set the matrix dimensions to (size1, size2)
      * a copy from NTL::Mat<T>::SetDims
      */
      void resize(size_type size1, size_type size2) { this->SetDims(size1, size2); }

      /**
      * release space and set to length 0
      * a copy from NTL::Mat<T>::kill
      */
      void clear() { this->kill(); } 

   
      /**
      * return the number of rows
      */
      size_type size1() const { return this->NumRows(); }

      /**
      * return the number of columns
      */
      size_type size2() const { return this->NumCols(); }

      /**
      * change the indexation reference for ()() operator to start from 0
      * in NTL::Vec<T> the ()() operator starts from 1 wich is not compatible with boost 
      */
      T& operator()(size_type i, size_type j) { return (*this)[i][j]; }
         const T& operator()(size_type i, size_type j) const { return (*this)[i][j]; }

   };


   /**
   * comparing NTL::GF2X objects
   */
   inline bool operator< (const GF2X& x, const GF2X& y){ 
   
         if(deg(x)!= deg(y)){
            return (deg(x) < deg(y));
         }
         for(long i= deg(x); i>=0; i--){
            if(coeff(x,i) != coeff(y,i) )
                  return IsOne(coeff(y,i));

         }
         return false;
   }

   /**
   * comparing NTL::GF2E objects
   */
   inline bool operator< (const GF2E& x, const GF2E& y){ 
         GF2X xx = rep(x);
         GF2X yy = rep(y);
         return (xx < yy) ;
   }

  


   // matrix proxy
   template <class M>
   class matrix_row : public vector<typename M::value_type> {
   public:
      inline matrix_row(M& data, typename M::size_type i) { this->_vec__rep = (typename M::value_type*&) data[i]._vec__rep; }
      inline ~matrix_row() { this->_vec__rep = 0; /* avoid destruction in parent class */ }
   };
}

#endif // __NTLWRAP_H__
