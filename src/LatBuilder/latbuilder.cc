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

#include "latbuilder/Parser/EmbeddingType.h"
#include "latbuilder/Parser/Lattice.h"
#include "latbuilder/Parser/CommandLine.h"   
#include "latbuilder/TextStream.h"
#include "latbuilder/Types.h"

#include "netbuilder/DigitalNet.h"
#include "netbuilder/Parser/OutputFormat.h"
#include "netbuilder/Types.h"

#include <fstream>
#include <chrono>

namespace LatBuilder{
using TextStream::operator<<;

static unsigned int merit_digits_displayed = 0; 

template <LatticeType LR, EmbeddingType ET>
void onLatticeSelected(const Task::Search<LR, ET>& s)
   {
   unsigned int old_precision = (unsigned int) std::cout.precision();
   if (merit_digits_displayed)
      std::cout.precision(merit_digits_displayed);
   std::cout << "==> " << s.bestLattice() << ": " << s.bestMeritValue() << std::endl;
   if (merit_digits_displayed)
      std::cout.precision(old_precision);
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
   ("set-type,T", po::value<std::string>(),
    "(required) point set type; possible values:\n"
    "  lattice\n"
    "  net\n")
   ("help,h", "produce help message")
   ("version,V", "show version")
   ("verbose,v", po::value<int>()->default_value(0),
   "specify the verbosity of the program\n")
  //  ("quiet,q", "show compact output (single line with number of points, generating vector and merit value)")
   ("construction,c", po::value<std::string>()->default_value("ordinary"),
   "lattice construction; possible values:\n"
   "  ordinary (default)\n"
   "  polynomial\n")
   ("multilevel,m", po::value<std::string>()->default_value("false"),
    "multilevel point set; possible values:\n"
   "  false (default)\n"
   "  true\n")
   ("size-parameter,s", po::value<std::string>(),
    "(required) modulus of the lattice; possible values:\n"
   "  <modulus>\n"
   "  <base>^<max-power>\n"
   "  input format :\n"
   "  ordinary lattice rules: integer (decimal reprisentation)\n"
   "  polynomial lattice rules: polynomial (list of coefficients: 1011 stands for 1 + X^2 + X^3)\n")
   ("exploration-method,E", po::value<std::string>(),
    "(required) exploration method; possible values:\n"
    "  evaluation:<a1>,...,<as>\n" // TODO change name to evaluation (and in python-wrapper)
    "  exhaustive\n"
    "  random:<r>\n"
    "  Korobov\n"
    "  random-Korobov:<r>\n"
    "  full-CBC\n"
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
    "  projection-dependent:<proj-1>:<weight-1>[:<proj-2>:<weight-2>[:...]]\n"
    "    where <proj-n> is a comma-separated list of coordinates\n"
    "  file:\"<file>\"\n"
    "    line format in <file>:\n"
    "      <i1>,<i2>,...: <weight>\n"
    "      order <x>: <weight>\n"
    "      default: <weight>\n"
    "    if <file> is `-' data is read from standard input\n")
   ("weights-power,o", po::value<Real>(),
    "(default: same value as for the --norm-type argument) real number specifying that the weights passed as input will be assumed to be already elevated at that power (a value of `inf' is mapped to 1)\n")
   ("norm-type,p", po::value<std::string>()->default_value("2"),
    "(default: 2) norm type used to combine the value of the projection-dependent figure of merit for all projections; possible values:"
    "    <p>: a real number corresponding the l_<p> norm\n"
    "    inf: corresponding to the `max' norm\n")
   ("figure-of-merit,M", po::value<std::string>(),
    "(required) type of figure of merit; format: [CU:]<merit>\n"
    "  where the optional \"CU:\" prefix switches on the coordinate-uniform evaluation algorithm,\n"
    "  and where <merit> is one of:\n"
    "    spectral\n"
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
   ("combiner,b", po::value<std::string>(),
    "(required for embedded) combiner for (filtered) multilevel merit values; possible values:\n"
    "  sum\n"
    "  max\n"
    "  level:{<level>|max}\n")
   ("repeat,r", po::value<unsigned int>()->default_value(1),
    "(optional) number of times the exploration must be executed\n"
   "(can be useful to obtain different results from random exploration)\n")
    ("output-format,g", po::value< std::vector<std::string> >()->composing(),
    "(optional) output generator matrices of the resulting polynomial lattice as a digital net, in the indicated format; possible values:\n"
   "  file:\"<file>\":format\n"
   "  available output formats\n"
   "  - ssj \n"
   "  - cli \n"
   "  - gui \n")
   ("merit-digits-displayed", po::value<unsigned int>()->default_value(0),
    "(optional) number of significant figures to use when displaying merit values\n");

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

  if (opt.count("combiner") < 1 && opt["multilevel"].as<std::string>() == "true"){
    throw std::runtime_error("--combiner must be specified for embedded (try --help)");
  }

   if (opt.count("weights") < 1)
      throw std::runtime_error("--weights must be specified (try --help)");
   for (const auto x : {"size-parameter", "exploration-method", "dimension", "figure-of-merit"}) {
      if (opt.count(x) != 1)
         throw std::runtime_error("--" + std::string(x) + " must be specified exactly once (try --help)");
   }

   return opt;
}


template <EmbeddingType ET>
void executeOrdinary(const Parser::CommandLine<LatticeType::ORDINARY, ET>& cmd, int verbose, unsigned int repeat)
{
   const LatticeType LR = LatticeType::ORDINARY ;
   using namespace std::chrono;

   auto search = cmd.parse();

   const std::string separator = "\n--------------------------------------------------------------------------------\n";

   if (verbose > 0) {
      search->onLatticeSelected().connect(onLatticeSelected<LR,ET>);
      search->setObserverVerbosity(verbose-1);
      std::cout << *search << std::endl;
   }

   for (unsigned int i = 0; i < repeat; i++) {

      if (verbose > 0)
         std::cout << separator << std::endl;

      auto t0 = high_resolution_clock::now();
      search->execute();
      auto t1 = high_resolution_clock::now();

      unsigned int old_precision = (unsigned int) std::cout.precision();
      if (merit_digits_displayed)
   std::cout.precision(merit_digits_displayed);
     const auto lat = search->bestLattice();
     
      if (verbose > 0) {
   auto dt = duration_cast<duration<double>>(t1 - t0);
         std::cout << std::endl;
         std::cout << "BEST LATTICE: " << search->bestLattice() << ": " << search->bestMeritValue() << std::endl;
         std::cout << "ELAPSED CPU TIME: " << dt.count() << " seconds" << std::endl;
      }
      else {
         std::cout << lat.sizeParam().modulus();
         for (const auto& a : lat.gen())
            std::cout << "\t" << a;
         std::cout << "\t" << search->bestMeritValue() << std::endl;
      }

      
      if (merit_digits_displayed)
   std::cout.precision(old_precision);

      search->reset();
    }

   if (verbose > 0)
      std::cout << separator << std::endl;
}


template <EmbeddingType ET>
void executePolynomial(const Parser::CommandLine<LatticeType::POLYNOMIAL, ET>& cmd, int verbose, unsigned int repeat, const std::vector<NetBuilder::Parser::OutputFormatParameters>& vecOutputFormatParameters)
{
   const LatticeType LR = LatticeType::POLYNOMIAL ;
   using namespace std::chrono;

   auto search = cmd.parse();

   const std::string separator = "\n--------------------------------------------------------------------------------\n";

   if (verbose > 0) {
      search->onLatticeSelected().connect(onLatticeSelected<LR,ET>);
      search->setObserverVerbosity(verbose-1);
      std::cout << *search << std::endl;
   }

   for (unsigned int i = 0; i < repeat; i++) {

        if (verbose > 0)
           std::cout << separator << std::endl;

        auto t0 = high_resolution_clock::now();
        search->execute();
        auto t1 = high_resolution_clock::now();

        unsigned int old_precision = (unsigned int) std::cout.precision();
        if (merit_digits_displayed){
          std::cout.precision(merit_digits_displayed);
        }
       const auto lat = search->bestLattice();
      

        if (verbose > 0) {
        auto dt = duration_cast<duration<double>>(t1 - t0);
           std::cout << std::endl;
           std::cout << "BEST LATTICE: " << lat << ": " << search->bestMeritValue() << std::endl;
           std::cout << "ELAPSED CPU TIME: " << dt.count() << " seconds" << std::endl;
        }
        else {
           std::cout << lat.sizeParam().modulus();
           for (const auto& a : lat.gen())
              std::cout << "\t" << a;
           std::cout << "\t" << search->bestMeritValue() << std::endl;
        }
        
        if (i == 0){ 
          NetBuilder::DigitalNetConstruction<NetBuilder::NetConstruction::POLYNOMIAL> net((unsigned int) lat.gen().size(), lat.sizeParam().modulus(),lat.gen());

          for (NetBuilder::Parser::OutputFormatParameters outputFormatParameters : vecOutputFormatParameters){
            std::string fileName = outputFormatParameters.file();
            ofstream outFile;
            outFile.open(fileName);
            outFile << net.format(outputFormatParameters.outputFormat());
            outFile.close();
          }
        }
        
        if (merit_digits_displayed){
          std::cout.precision(old_precision);
        }
          
        search->reset();
    }

   if (verbose > 0)
      std::cout << separator << std::endl;
}

   
 




int main(int argc, const char *argv[])
{

   try {
        auto opt = parse(argc, argv);

        // bool quiet = opt.count("quiet");
        int verbose = opt["verbose"].as<int>();
        
        auto repeat = opt["repeat"].as<unsigned int>();

        // global variable
        merit_digits_displayed = opt["merit-digits-displayed"].as<unsigned int>();

       LatBuilder::LatticeType lattice = Parser::LatticeParser::parse(opt["construction"].as<std::string>());

       if(lattice == LatticeType::ORDINARY){

            Parser::CommandLine<LatticeType::ORDINARY, EmbeddingType::MULTILEVEL> cmd;

            

            cmd.construction  = opt["exploration-method"].as<std::string>();
            cmd.size          = opt["size-parameter"].as<std::string>();
            cmd.dimension     = opt["dimension"].as<std::string>();
            cmd.normType      = opt["norm-type"].as<std::string>();
            cmd.figure        = opt["figure-of-merit"].as<std::string>();
            cmd.weights       = opt["weights"].as<std::vector<std::string>>();
            if (opt.count("combiner") == 1){
              cmd.combiner      = opt["combiner"].as<std::string>();
            }
            else{
              cmd.combiner = "level:max";
            }

            cmd.weightsPowerScale = 1.0;
            if (opt.count("weights-power") >= 1) {
               // assume 1.0 if norm-type is `inf' or anything else
               cmd.weightsPowerScale = 1.0;
               try {
                  // start the value of norm-type as a default
                  if (cmd.normType != "inf")
                     cmd.weightsPowerScale = boost::lexical_cast<Real>(cmd.normType);
               }
               catch (boost::bad_lexical_cast&) {}
               // then scale down according to interpretation of input
               cmd.weightsPowerScale /= opt["weights-power"].as<Real>();
            }

            if (opt.count("filters") >= 1)
               cmd.filters = opt["filters"].as<std::vector<std::string>>();

            if (opt.count("multilevel-filters") >= 1)
               cmd.multilevelFilters = opt["multilevel-filters"].as<std::vector<std::string>>();

            EmbeddingType latType = Parser::EmbeddingType::parse(opt["multilevel"].as<std::string>());

            if (latType == EmbeddingType::UNILEVEL){

               executeOrdinary<EmbeddingType::UNILEVEL> (cmd, verbose, repeat);
               
             }
            else{
               executeOrdinary<EmbeddingType::MULTILEVEL> (cmd, verbose, repeat);
               
             }
      }

      else if(lattice == LatticeType::POLYNOMIAL){

            Parser::CommandLine<LatticeType::POLYNOMIAL, EmbeddingType::MULTILEVEL> cmd;

            

            cmd.construction  = opt["exploration-method"].as<std::string>();
            cmd.size          = opt["size-parameter"].as<std::string>();
            cmd.dimension     = opt["dimension"].as<std::string>();
            cmd.normType      = opt["norm-type"].as<std::string>();
            cmd.figure        = opt["figure-of-merit"].as<std::string>();
            cmd.weights       = opt["weights"].as<std::vector<std::string>>();
            if (opt.count("combiner") == 1){
              cmd.combiner      = opt["combiner"].as<std::string>();
            }
            else{
              cmd.combiner = "level:max";
            }

            cmd.weightsPowerScale = 1.0;
            if (opt.count("weights-power") >= 1) {
               // assume 1.0 if norm-type is `inf' or anything else
               cmd.weightsPowerScale = 1.0;
               try {
                  // start the value of norm-type as a default
                  if (cmd.normType != "inf")
                     cmd.weightsPowerScale = boost::lexical_cast<Real>(cmd.normType);
               }
               catch (boost::bad_lexical_cast&) {}
               // then scale down according to interpretation of input
               cmd.weightsPowerScale /= opt["weights-power"].as<Real>();
            }

            if (opt.count("filters") >= 1)
               cmd.filters = opt["filters"].as<std::vector<std::string>>();

            if (opt.count("multilevel-filters") >= 1)
               cmd.multilevelFilters = opt["multilevel-filters"].as<std::vector<std::string>>();

            EmbeddingType latType = Parser::EmbeddingType::parse(opt["multilevel"].as<std::string>());

            std::vector<NetBuilder::Parser::OutputFormatParameters> outPoly;
            if (opt.count("output-format") >= 1){
              outPoly = NetBuilder::Parser::OutputFormatParser::parse(opt["output-format"].as<std::vector<string>>());
            }
            else{
              outPoly = {};
            }

            if (latType == EmbeddingType::UNILEVEL){
               executePolynomial< EmbeddingType::UNILEVEL> (cmd, verbose, repeat, outPoly);
               
             }
            else{
               executePolynomial<EmbeddingType::MULTILEVEL> (cmd, verbose, repeat, outPoly);
               
             }
      }
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

}