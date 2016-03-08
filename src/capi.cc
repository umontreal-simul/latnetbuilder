#include "latbuilder/capi.h"

#include "latbuilder/Types.h"
#include "latbuilder/Parser/LatType.h"
#include "latbuilder/Parser/CommandLine.h"
#include "latbuilder/TextStream.h"

#include <chrono>

#ifndef LATBUILDER_VERSION
#define LATBUILDER_VERSION "(unkown version)"
#endif

using namespace LatBuilder;
using TextStream::operator<<;

#define FOR_TYPE(type, ord, emb, member) (type == LatType::ORDINARY ? ord.member : emb.member)

struct latbuilder_result_ {

  latbuilder_result_(const LatDef<LatType::ORDINARY>& lat, double merit = 0.0, double seconds = 0.0) :
    m_type(LatType::ORDINARY), m_ord(lat), m_emb(), m_merit(merit), m_seconds(seconds) {}
  latbuilder_result_(const LatDef<LatType::EMBEDDED>& lat, double merit = 0.0, double seconds = 0.0) :
    m_type(LatType::EMBEDDED), m_ord(), m_emb(lat), m_merit(merit), m_seconds(seconds) {}

  Modulus num_points() const { return FOR_TYPE(m_type, m_ord, m_emb, sizeParam().numPoints()); }
  Dimension dimension()const { return FOR_TYPE(m_type, m_ord, m_emb, dimension()); }
  const Modulus* gen() const { return FOR_TYPE(m_type, m_ord, m_emb, gen().data()); }
  double merit()       const { return m_merit; }
  double cpu_seconds() const { return m_seconds; }

  LatType m_type;
  LatDef<LatType::ORDINARY> m_ord;
  LatDef<LatType::EMBEDDED> m_emb;
  double m_merit;
  double m_seconds;
};


std::string latbuilder_error_string;

void latbuilder_release_result(latbuilder_result* result) {
  if (result) delete result;
}

const char* latbuilder_get_version_string() {
  return LATBUILDER_VERSION;
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

const unsigned long* latbuilder_result_get_gen(const latbuilder_result* result) {
  return result->gen();
}

double latbuilder_result_get_merit(const latbuilder_result* result) {
  return result->merit();
}

double latbuilder_result_get_cpu_seconds(const latbuilder_result* result) {
  return result->cpu_seconds();
}

template <LatType LAT>
void latbuilder_search_str_init_cmd_extra(
  Parser::CommandLine<LAT>& cmd,
  size_t multilevel_filter_count,
  const char** multilevel_filters,
  const char* combiner);

template <>
void latbuilder_search_str_init_cmd_extra<LatType::ORDINARY>(
  Parser::CommandLine<LatType::ORDINARY>& cmd,
  size_t multilevel_filter_count,
  const char** multilevel_filters,
  const char* combiner) {
}

template <>
void latbuilder_search_str_init_cmd_extra<LatType::EMBEDDED>(
  Parser::CommandLine<LatType::EMBEDDED>& cmd,
  size_t multilevel_filter_count,
  const char** multilevel_filters,
  const char* combiner) {

    for (size_t i = 0; i < multilevel_filter_count; i++)
      cmd.multilevelFilters.push_back(multilevel_filters[i]);

    cmd.combiner = combiner;
}

template <LatType LAT>
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

      Parser::CommandLine<LAT> cmd;

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

    return search_str<LatType::ORDINARY>(construction, size, dimension, norm_type, figure, weight_count, weights, weights_power, filter_count, filters, 0, NULL, NULL, result);
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

    return search_str<LatType::EMBEDDED>(construction, size, dimension, norm_type, figure, weight_count, weights, weights_power, filter_count, filters, multilevel_filter_count, multilevel_filters, combiner, result);
}

// template <LatType LAT>
// struct latbuilder_search_str_base;
//
// template <>
// struct latbuilder_search_str_base<LatType::ORDINARY> {
//   void init_cmd_extra(
//     Parser::CommandLine<LatType::ORDINARY>& cmd,
//     size_t multilevel_filter_count,
//     const char** multilevel_filters,
//     const char* combiner) {
//   }
// };
//
// template <>
// struct latbuilder_search_str_base<LatType::EMBEDDED> {
//   void init_cmd_extra(
//     Parser::CommandLine<LatType::EMBEDDED>& cmd,
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
