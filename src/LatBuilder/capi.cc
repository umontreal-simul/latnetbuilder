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

#include "latbuilder/capi.h"

#include "latbuilder/Types.h"
#include "latbuilder/Parser/PointSetType.h"
#include "latbuilder/Parser/CommandLine.h"
#include "latbuilder/TextStream.h"

#include <chrono>

#ifndef LATNETBUILDER_VERSION
#define LATNETBUILDER_VERSION "(unkown version)"
#endif

using namespace LatBuilder;
using TextStream::operator<<;

#define FOR_TYPE(type, ord, emb, member) (type == PointSetType::UNILEVEL ? ord.member : emb.member)
#define LR LatticeType::ORDINARY

struct latbuilder_result_ {

  typedef LatticeTraits<LR>::Modulus Modulus;

  latbuilder_result_(const LatDef<LR, PointSetType::UNILEVEL>& lat, double merit = 0.0, double seconds = 0.0) :
    m_type(PointSetType::UNILEVEL), m_ord(lat), m_emb(), m_merit(merit), m_seconds(seconds) {}
  latbuilder_result_(const LatDef<LR, PointSetType::MULTILEVEL>& lat, double merit = 0.0, double seconds = 0.0) :
    m_type(PointSetType::MULTILEVEL), m_ord(), m_emb(lat), m_merit(merit), m_seconds(seconds) {}

  uInteger num_points() const { return FOR_TYPE(m_type, m_ord, m_emb, sizeParam().numPoints()); }
  Modulus modulus() const { return FOR_TYPE(m_type, m_ord, m_emb, sizeParam().modulus()); }
  Dimension dimension()const { return FOR_TYPE(m_type, m_ord, m_emb, dimension()); }
  const Modulus* gen() const { return FOR_TYPE(m_type, m_ord, m_emb, gen().data()); }
  double merit()       const { return m_merit; }
  double cpu_seconds() const { return m_seconds; }

  PointSetType m_type;
  LatDef<LR, PointSetType::UNILEVEL> m_ord;
  LatDef<LR, PointSetType::MULTILEVEL> m_emb;
  double m_merit;
  double m_seconds;
};


std::string latbuilder_error_string;


void latbuilder_release_result(latbuilder_result* result) {
  if (result) delete result;
}

const char* latbuilder_get_version_string() {
  return LATNETBUILDER_VERSION;
}

const char* latbuilder_get_error_string() {
  return latbuilder_error_string.c_str();
}


unsigned long latbuilder_result_get_num_points(const latbuilder_result* result) {
  return result->num_points();
}


size_t latbuilder_result_get_dimension(const latbuilder_result* result) {
  return result->dimension();
}


const typename LatticeTraits<LR>::Modulus* latbuilder_result_get_gen(const latbuilder_result* result) {
  return result->gen();
}


double latbuilder_result_get_merit(const latbuilder_result* result) {
  return result->merit();
}


double latbuilder_result_get_cpu_seconds(const latbuilder_result* result) {
  return result->cpu_seconds();
}

template <PointSetType PST>
void latbuilder_search_str_init_cmd_extra(
  Parser::CommandLine<LR, PST>& cmd,
  size_t multilevel_filter_count,
  const char** multilevel_filters,
  const char* combiner);

template <>
void latbuilder_search_str_init_cmd_extra<PointSetType::UNILEVEL>(
  Parser::CommandLine<LR, PointSetType::UNILEVEL>& cmd,
  size_t multilevel_filter_count,
  const char** multilevel_filters,
  const char* combiner) {
}

template <>
void latbuilder_search_str_init_cmd_extra<PointSetType::MULTILEVEL>(
  Parser::CommandLine<LR, PointSetType::MULTILEVEL>& cmd,
  size_t multilevel_filter_count,
  const char** multilevel_filters,
  const char* combiner) {

    for (size_t i = 0; i < multilevel_filter_count; i++)
      cmd.multilevelFilters.push_back(multilevel_filters[i]);

    cmd.combiner = combiner;
}

template <PointSetType PST>
static
latbuilder_status search_str(
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
  latbuilder_result** result) {

    try {

      Parser::CommandLine<LR, PST> cmd;

      cmd.construction = construction;
      cmd.size         = size;
      cmd.dimension    = dimension;
      cmd.normType     = norm_type;
      cmd.figure       = figure;

      for (size_t i = 0; i < weight_count; i++)
        cmd.weights.push_back(weights[i]);

      cmd.weightsPowerScale = 1.0;

      // assume 1.0 if norm-type is `inf' or anything else
      cmd.weightsPowerScale = 1.0;
      try {
         // start the value of norm-type as a default
         if (cmd.normType != "inf")
            cmd.weightsPowerScale = boost::lexical_cast<Real>(cmd.normType);
      }
      catch (boost::bad_lexical_cast&) {}
      // then scale down according to interpretation of input
      cmd.weightsPowerScale /= boost::lexical_cast<Real>(weights_power);

      for (size_t i = 0; i < filter_count; i++)
        cmd.filters.push_back(filters[i]);

      latbuilder_search_str_init_cmd_extra(cmd, multilevel_filter_count, multilevel_filters, combiner);

      using namespace std::chrono;

      auto search = cmd.parse();

      auto t0 = high_resolution_clock::now();
      search->execute();
      auto t1 = high_resolution_clock::now();
      auto dt = duration_cast<duration<double>>(t1 - t0);

      *result = new latbuilder_result(search->bestLattice(), search->bestMeritValue(), dt.count());

      return LATBUILDER_OK;
    }
    catch (Parser::ParserError& e) {
      latbuilder_error_string = e.what();
    }
    catch (std::exception& e) {
      latbuilder_error_string = e.what();
    }
    return LATBUILDER_ERROR;
}


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
  latbuilder_result** result) {

    return search_str<PointSetType::UNILEVEL>(construction, size, dimension, norm_type, figure, weight_count, weights, weights_power, filter_count, filters, 0, NULL, NULL, result);
}


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
  latbuilder_result** result) {

    return search_str<PointSetType::MULTILEVEL>(construction, size, dimension, norm_type, figure, weight_count, weights, weights_power, filter_count, filters, multilevel_filter_count, multilevel_filters, combiner, result);
}

// template <PointSetType PST>
// struct latbuilder_search_str_base;
//
// template <>
// struct latbuilder_search_str_base<PointSetType::UNILEVEL> {
//   void init_cmd_extra(
//     Parser::CommandLine<PointSetType::UNILEVEL>& cmd,
//     size_t multilevel_filter_count,
//     const char** multilevel_filters,
//     const char* combiner) {
//   }
// };
//
// template <>
// struct latbuilder_search_str_base<PointSetType::MULTILEVEL> {
//   void init_cmd_extra(
//     Parser::CommandLine<PointSetType::MULTILEVEL>& cmd,
//     size_t multilevel_filter_count,
//     const char** multilevel_filters,
//     const char* combiner) {
//
//       for (size_t i = 0; i < multilevel_filter_count; i++)
//         cmd.multilevel_filters.push_back(multilevel_filters[i]);
//
//       cmd.combiner = combiner;
//   }
// };