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

#include "latbuilder/Parser/LatType.h"
#include "latbuilder/Parser/CommandLine.h"
#include "latbuilder/TextStream.h"

#include <boost/chrono/chrono_io.hpp>

#ifndef VERSION
#define VERSION "(unkown version)"
#endif

using namespace LatBuilder;
using TextStream::operator<<;

typedef boost::chrono::high_resolution_clock Clock;
typedef boost::chrono::time_point<boost::chrono::high_resolution_clock, boost::chrono::duration<double, boost::ratio<1>>> TimePoint;


template <LatType LAT>
void onLatticeSelected(const Task::Search<LAT>& s)
{
   std::cout << "==> " << s.bestLattice() << ": " << s.bestMeritValue() << std::endl;
   const auto accepted = s.minObserver().acceptedCount();
   const auto rejected = s.minObserver().rejectedCount();
   const auto total = s.minObserver().totalCount();
   std::cout << "    accepted/rejected/total: "
      << accepted << "/" << rejected << "/" << total << std::endl;
}

boost::program_options::options_description
makeOptionsDescription()
{
   namespace po = boost::program_options;
   po::options_description desc("allowed options");

   desc.add_options ()
   ("help,h", "produce help message")
   ("version,h", "show version")
   ("quiet,q", "show compact output (single line with number of points, generating vector and merit value)")
   ("lattice-type,l", po::value<std::string>()->default_value("ordinary"),
    "type of lattice; possible values:\n"
	"  ordinary (default)\n"
	"  embedded\n")
   ("size,n", po::value<std::string>(),
    "(required) number of points in the lattice; possible values:\n"
	"  <num-points>\n"
	"  <base>^<max-power>\n")
   ("construction,c", po::value<std::string>(),
    "(required) construction type; possible values:\n"
    "  explicit:<a1>,...,<as>\n"
    "  exhaustive\n"
    "  random:<r>\n"
    "  Korobov\n"
    "  random-Korobov:<r>\n"
    "  CBC\n"
    "  random-CBC:<r>\n"
    "  fast-CBC\n"
    "  extend:<num-points>:<a1>,...,<as>\n"
    "where <r> is the number of randomizations, "
    "and <a1>,...,<as> are the components of the generating vector\n")
   ("weights,w", po::value<std::vector<std::string>>()->multitoken(),
    "(required) whitespace-separated list of weights specifications (the actual weights are the sum of these); possible values:\n"
    "  product:<default>:<coord1-weight>[,...]\n"
    "  order-dependent:<default>:<order-1-weight>,...,<order-s-weight>\n"
    "  POD:<default>:<order-1-weight>,...,<order-s-weight>:<default>:<coord1-weight>[,...]\n"
    "  projection-dependent:\"<file>\"\n"
    "    line format in <file>:\n"
    "      <i1>,<i2>,...: <weight>\n"
    "      order <x>: <weight>\n"
    "      default: <weight>\n"
    "    if <file> is `-' data is read from standard input\n")
   ("figure-of-merit,m", po::value<std::string>(),
    "(required) type of figure of merit; format: [CS:]<accumulator>:<merit>\n"
    "  where the optional \"CS:\" prefix switches on the coordinate-symmetric evaluation algorithm,\n"
    "  where <accumulator> is either:\n"
    "    sum\n"
    "    max\n"
    "  and where <merit> is one of:\n"
    "    spectral:<power>\n"
    "    P<alpha>\n"
    "    R<alpha>\n")
   ("dimension,d", po::value<std::string>(),
    "(required) lattice dimension\n")
   ("filters,f", po::value<std::vector<std::string>>()->multitoken(),
    "whitespace-separated list of filters for merit values; possible values:\n"
    "  norm:P<alpha>-{SL10|DPW08}\n"
    "  low-pass:<threshold>\n")
   ("multilevel-filters,F", po::value<std::vector<std::string>>()->multitoken(),
    "whitespace-separated list of filters for multilevel merit values; possible values:\n"
    "  norm:P<alpha>-{SL10|DPW08}[:<multilevel-weights>]\n"
    "  low-pass:<threshold>\n"
    "where <multilevel-weights> specifies the per-level weights; possible values:\n"
    "  even[:<min-level>[:<max-level>]] (default)\n")
   ("combiner,b", po::value<std::string>()->default_value("level:max"),
    "combiner for (filtered) multilevel merit values; possible values:\n"
    "  sum\n"
    "  max\n"
    "  level:{<level>|max}\n")
   ("repeat,r", po::value<unsigned int>()->default_value(1),
    "(optional) number of times the construction must be executed\n"
	"(can be useful to obtain different results from random constructions)\n");

   return desc;
}


boost::program_options::variables_map
parse(int argc, const char* argv[])
{
   namespace po = boost::program_options;

   auto desc = makeOptionsDescription();

   po::variables_map opt;
   po::store(po::parse_command_line(argc, argv, desc), opt);
   po::notify(opt);

   if (opt.count("help")) {
      std::cout << desc << std::endl;
      std::exit (0);
   }

   if (opt.count("version")) {
      std::cout << "Lattice Builder " << VERSION << std::endl;
      std::exit (0);
   }

   if (opt.count("weights") < 1)
      throw std::runtime_error("--weights must be specified (try --help)");
   for (const auto x : {"size", "construction", "dimension", "figure-of-merit"}) {
      if (opt.count(x) != 1)
         throw std::runtime_error("--" + std::string(x) + " must be specified exactly once (try --help)");
   }

   return opt;
}


template <LatType LAT>
void execute(const Parser::CommandLine<LAT>& cmd, bool quiet, unsigned int repeat)
{
   auto search = cmd.parse();

   const std::string separator = "\n--------------------------------------------------------------------------------\n";

   if (not quiet) {
      search->onLatticeSelected().connect(onLatticeSelected<LAT>);
      std::cout << *search << std::endl;
   }

   for (unsigned int i = 0; i < repeat; i++) {

      if (not quiet)
         std::cout << separator << std::endl;

      TimePoint t0 = Clock::now();
      search->execute();
      TimePoint t1 = Clock::now();

      if (not quiet) {
         std::cout << std::endl;
         std::cout << "BEST LATTICE: " << search->bestLattice() << ": " << search->bestMeritValue() << std::endl;
         std::cout << "ELAPSED CPU TIME: " << (t1 - t0) << std::endl;
      }
      else {
         const auto lat = search->bestLattice();
         std::cout << lat.sizeParam().numPoints();
         for (const auto& a : lat.gen())
            std::cout << "\t" << a;
         std::cout << "\t" << search->bestMeritValue() << std::endl;
      }

      search->reset();
   }

   if (not quiet)
      std::cout << separator << std::endl;
}

int main(int argc, const char *argv[])
{

   try {

      Parser::CommandLine<LatType::EMBEDDED> cmd;

      auto opt = parse(argc, argv);

      bool quiet = opt.count("quiet");

      auto repeat = opt["repeat"].as<unsigned int>();

      cmd.construction  = opt["construction"].as<std::string>();
      cmd.size          = opt["size"].as<std::string>();
      cmd.dimension     = opt["dimension"].as<std::string>();
      cmd.figure        = opt["figure-of-merit"].as<std::string>();
      cmd.weights       = opt["weights"].as<std::vector<std::string>>();
      cmd.combiner      = opt["combiner"].as<std::string>();

      if (opt.count("filters") >= 1)
         cmd.filters = opt["filters"].as<std::vector<std::string>>();

      if (opt.count("multilevel-filters") >= 1)
         cmd.multilevelFilters = opt["multilevel-filters"].as<std::vector<std::string>>();

      LatType latType = Parser::LatType::parse(opt["lattice-type"].as<std::string>());

      if (latType == LatType::ORDINARY)
         execute<LatType::ORDINARY>(cmd, quiet, repeat);
      else
         execute<LatType::EMBEDDED>(cmd, quiet, repeat);
   }
   catch (Parser::ParserError& e) {
      std::cerr << "COMMAND LINE ERROR: " << e.what() << std::endl;
      std::exit(1);
   }
   catch (std::exception& e) {
      std::cerr << "ERROR: " << e.what() << std::endl;
      std::exit(1);
   }

   return 0;
}
