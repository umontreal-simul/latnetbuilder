#ifndef LATBUILDER__CAPI_H
#define LATBUILDER__CAPI_H

#include <stddef.h>

/**
 *  \file capi.h
 *  C API for Lattice Builder
 *
 *  **IMPORTANT:** Programs that use the C API *must* link with the standard C++ library (libstdc++).
 */

#ifdef __cplusplus
extern "C" {
#endif

  /** Status types for the functions of the Lattice Builder C API. */
  typedef enum latbuilder_status_ {
    LATBUILDER_OK = 0,
    LATBUILDER_IN_PROGRESS = 1,
    LATBUILDER_ERROR = -1
  } latbuilder_status;

  struct latbuilder_result_;
  /** Structure used to store search results. */
  typedef struct latbuilder_result_ latbuilder_result;

  /** Return a string describing the current version of Lattice Builder. */
  const char* latbuilder_get_version_string();

  /** Return the error string associated with the last error that occurred. */
  const char* latbuilder_get_error_string();

  /** Release the memory resources associated with result. */
  void latbuilder_release_result(latbuilder_result* result);

  /** Return the number of points of the lattice. */
  unsigned long latbuilder_result_get_num_points(const latbuilder_result* result);

  /** Return the dimension of the lattice. */
  size_t latbuilder_result_get_dimension(const latbuilder_result* result);

  /** Return the generating vector (as an array) of the lattice. */
  const unsigned long* latbuilder_result_get_gen(const latbuilder_result* result);

  /** Return the merit value of the lattice. */
  double latbuilder_result_get_merit(const latbuilder_result* result);

  /** Return the CPU time (in seconds) used to construct the lattice */
  double latbuilder_result_get_cpu_seconds(const latbuilder_result* result);

  /**
   * Search for an ordinary lattice rule using string arguments.
   *
   * @return LATBUILDER_OK on success, or LATBUILDER_ERROR on error (see latbuilder_get_error_string()).
   */
  latbuilder_status latbuilder_search_ordinary_str(
    const char* construction,
    const char* size,
    const char* dimension,
    const char* norm_type,
    const char* figure,
    size_t weight_count,
    const char** weights,
    const char* weights_power,
    size_t filter_count,
    const char** filters,
    latbuilder_result** result);


  /**
   * Search for embedded lattice rules using string arguments.
   *
   * @return LATBUILDER_OK on success, or LATBUILDER_ERROR on error (see latbuilder_get_error_string()).
   */
  latbuilder_status latbuilder_search_embedded_str(
    const char* construction,
    const char* size,
    const char* dimension,
    const char* norm_type,
    const char* figure,
    size_t weight_count,
    const char** weights,
    const char* weights_power,
    size_t filter_count,
    const char** filters,
    size_t multilevel_filter_count,
    const char** multilevel_filters,
    const char* combiner,
    latbuilder_result** result);

#ifdef __cplusplus
}
#endif

#endif /* LATBUILDER_H */
