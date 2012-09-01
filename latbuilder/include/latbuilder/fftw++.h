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

/**
 * \file
 * Wrapper for a subset of FFTW functions.
 */

#ifndef __FFTW_XX_H__
#define __FFTW_XX_H__

#ifdef FFTWXX_USE_BOOST_VECTOR
#include <boost/numeric/ublas/vector.hpp>
#else
#include <vector>
#endif

#include <stdexcept>
#include <complex>
#include <fftw3.h>


/**
 * Wrapper for a subset of FFTW: FFT's for real functions in one dimension.
 */
template <typename T>
struct fftw
{
   /**
    * STL allocator replacement using FFTW's memory allocation functions.
    * They ensure proper memory alignment for the use of SIMD processor
    * instructions.
    */
   template <typename Tp>
   class allocator
   {
   public:
      typedef size_t     size_type;
      typedef ptrdiff_t  difference_type;
      typedef Tp*        pointer;
      typedef const Tp*  const_pointer;
      typedef Tp&        reference;
      typedef const Tp&  const_reference;
      typedef Tp         value_type;

      template<typename Tp1> struct rebind { typedef allocator<Tp1> other; };

      allocator() throw() { }
      allocator(const allocator&) throw() { }
      template<typename Tp1> allocator(const allocator<Tp1>&) throw() { }
      ~allocator() throw() { }

      pointer allocate(size_type n, const void* = 0)
      { return static_cast<Tp*>(c_api::malloc(n * sizeof(Tp))); }

      void deallocate(pointer p, size_type) { c_api::free(p); }

      constexpr size_type max_size() const throw() { return size_t(-1) / sizeof(Tp); }

      void construct(pointer p) { ::new((void *)p) Tp(); }
      void construct(pointer p, const Tp& val) { ::new((void *)p) Tp(val); }
      void destroy(pointer p) { p->~Tp(); }
   };

   /// Low-level wrapper for the C API of FFTW.
   struct c_api;

   /// Real number.
   typedef T real;

   /// Complex number.
   typedef std::complex<T> complex;

#ifdef FFTWXX_USE_BOOST_VECTOR
   typedef std::vector<real, allocator<real> > real_storage;
   typedef std::vector<complex, allocator<complex> > complex_storage;

   /// Vector of real numbers using FFTW allocation.
   typedef boost::numeric::ublas::vector<real, real_storage> real_vector;

   /// Vector of complex numbers using FFTW allocation.
   typedef boost::numeric::ublas::vector<complex, complex_storage> complex_vector;
#else
   /// Vector of real numbers using FFTW allocation.
   typedef std::vector<real, allocator<real> > real_vector;

   /// Vector of complex numbers using FFTW allocation.
   typedef std::vector<complex, allocator<complex> > complex_vector;
#endif

   /**
    * Computes the real-to-complex Fourier transform of \c v into \c result.
    * The size of the transform is that of the real component.
    * The expected size of result is \c v.size() / 2 + 1.
    * Because result is Hermitian, only half of the elements (plus one) are stored.
    *
    */
   static complex_vector& fft(const real_vector& v, complex_vector& result)
   {
      if (result.size() < fft_size(v))
         throw std::invalid_argument("fftw::fft(): result must have size v.size() / 2 + 1");
      // the transform is performed out-of-place, hence the const_cast is safe
      typename c_api::plan p = c_api::plan_dft_r2c_1d(
            v.size(),
            const_cast<typename real_vector::value_type*>(&v[0]),
            &result[0],
            FFTW_ESTIMATE);
      c_api::execute(p);
      c_api::destroy_plan(p);
      return result;
   }

   /**
    * Returns the output size (exact) for real-to-complex Fourier transform of \c v.
    * \sa transform(const real_vector&, complex_vector&)
    *
    */
   inline static typename complex_vector::size_type fft_size(const real_vector& v)
   {
      return v.size() / 2 + 1;
   }

   /**
    * Computes the real-to-complex Fourier transform of \c v.
    * \sa transform(const real_vector&, complex_vector&)
    *
    */
   static complex_vector fft(const real_vector& v)
   {
      complex_vector fv(fft_size(v));
      fft(v, fv);
      return fv;
   }

   /**
    * Computes the complex-to-real Fourier transform of \c v into \c result.
    * The size of the transform is that of the real component.
    * The expected size of \c v is is \c result.size() / 2 + 1.
    * Because v is Hermitian, only half of the elements (plus one) are
    * considered.
    *
    * \warning If normalize is \c false, the \c result vector is  not
    * normalized; its components must be divided by \c result.size() for proper
    * normalization.
    *
    */
   static real_vector& ifft(const complex_vector& v, real_vector& result, bool normalize=true)
   {
      if (v.size() < fft_size(result))
         throw std::invalid_argument("fftw::ifft(): v must have size result.size() / 2 + 1");
      // the transform is performed out-of-place, hence the const_cast is safe
      typename c_api::plan p = c_api::plan_dft_c2r_1d(
            result.size(),
            const_cast<typename complex_vector::value_type*>(&v[0]),
            &result[0],
            FFTW_ESTIMATE);
      c_api::execute(p);
      c_api::destroy_plan(p);
      if (normalize) {
         real norm = static_cast<real>(1.0 / result.size());
         for (typename real_vector::iterator it = result.begin(); it != result.end(); ++it)
            *it *= norm;
      }
      return result;
   }

   /**
    * Returns the output size (guessed) for complex-to-real Fourier transform of \c v.
    * \sa transform(const complex_vector&, real_vector&)
    */
   inline static typename real_vector::size_type real_ifft_size(const complex_vector& v)
   {
      return 2 * (v.size() - 1);
   }

   /**
    * Computes the complex-to-real Fourier transform of \c v.
    * Assumes the size of the transform is 2 * (\c v.size() - 1).
    * Returns a vector of size 2 * (\c v.size() - 1).
    * \sa transform(const complex_vector&, real_vector&)
    */
   static real_vector real_ifft(const complex_vector& v, bool normalize=true)
   {
      real_vector fv(real_ifft_size(v));
      ifft(v, fv, normalize);
      return fv;
   }
};

/**
 * Specialization of \struct fftw<>::c_api for \c float precision.
 */
template <>
struct fftw<float>::c_api
{
   typedef float real;
   typedef std::complex<float> complex;
   typedef fftwf_plan plan;

   static void *malloc(size_t n)
   { return fftwf_malloc(n); }

   static void free(void *p)
   { fftwf_free(p); }

   // this works as long as the data in std::complex is [real, imag]
   static plan plan_dft_r2c_1d(int n, real *in, complex *out, unsigned flags)
   { return fftwf_plan_dft_r2c_1d(n, in, reinterpret_cast<fftwf_complex*>(out), flags); }

   // this works as long as the data in std::complex is [real, imag]
   static plan plan_dft_c2r_1d(int n, complex *in, real *out, unsigned flags)
   { return fftwf_plan_dft_c2r_1d(n, reinterpret_cast<fftwf_complex*>(in), out, flags); }

   static void destroy_plan(plan p)
   { fftwf_destroy_plan(p); }

   static void execute(const plan p)
   { return fftwf_execute(p); }
};

/**
 * Specialization of c_api for \c double precision.
 */
template <>
struct fftw<double>::c_api
{
   typedef double real;
   typedef std::complex<double> complex;
   typedef fftw_plan plan;

   static void *malloc(size_t n)
   { return fftw_malloc(n); }

   static void free(void *p)
   { fftw_free(p); }

   // this works as long as the data in std::complex is [real, imag]
   static plan plan_dft_r2c_1d(int n, real *in, complex *out, unsigned flags)
   { return fftw_plan_dft_r2c_1d(n, in, reinterpret_cast<fftw_complex*>(out), flags); }

   // this works as long as the data in std::complex is [real, imag]
   static plan plan_dft_c2r_1d(int n, complex *in, real *out, unsigned flags)
   { return fftw_plan_dft_c2r_1d(n, reinterpret_cast<fftw_complex*>(in), out, flags); }

   static void destroy_plan(plan p)
   { fftw_destroy_plan(p); }

   static void execute(const plan p)
   { return fftw_execute(p); }
};


template <typename T1, typename T2>
inline bool operator==(
      const typename fftw<double>::template allocator<T1>&,
      const typename fftw<double>::template allocator<T2>&)
{ return true; }

template <typename T1, typename T2>
inline bool operator!=(
      const typename fftw<double>::template allocator<T1>&,
      const typename fftw<double>::template allocator<T2>&)
{ return false; }

#endif
