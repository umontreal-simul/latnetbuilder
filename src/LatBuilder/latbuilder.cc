// This file is part of LatNet Builder.
//
// Copyright (C) 2012-2021  The LatNet Builder author's, supervised by Pierre L'Ecuyer, Universite de Montreal.
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
#include "netbuilder/Types.h"

#include "netbuilder/Parser/OutputStyleParser.h"

#include <fstream>
#include <chrono>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string/join.hpp>

namespace LatBuilder{
using TextStream::operator<<;

static unsigned int merit_digits_displayed = 0; 

template <LatticeType LR, EmbeddingType ET>
void onLatticeSelected(const Task::Search<LR, ET>& s)
   {
     Dimension currentDim = s.bestLattice().dimension();
     Dimension totalDim = s.dimension();
      unsigned int old_precision = (unsigned int) std::cout.precision();
      if (merit_digits_displayed){
        std::cout.precision(merit_digits_displayed);
      }
      std::string lattice;
      if (s.minObserver().totalCount() == 1){
        lattice = " lattice";
      }
      else{
        lattice = " lattices";
      }
      
       std::cout << "End coordinate: " << currentDim << "/" << totalDim << " - "
       << s.minObserver().totalCount() << lattice  << " explored (" << s.minObserver().acceptedCount() << " accepted)"
       << " - partial merit value: " << s.bestMeritValue() << std::endl;
     
      if (merit_digits_displayed){
        std::cout.precision(old_precision);
      }
      if (currentDim < totalDim){
        std::cout << "Begin coordinate " << currentDim+1 << "/" << totalDim << std::endl;
      }
   }

boost::program_options::options_description
makeOptionsDescription()
{
   namespace po = boost::program_options;
   po::options_description desc("allowed options");

   desc.add_options ()
   ("help,h", "produce help message")
   ("version", "show version")
   ("set-type,t", po::value<std::string>(),
    "(required) point set type; possible values:\n"
    "  lattice\n"
    "  net\n")
   ("construction,c", po::value<std::string>(),
   "lattice construction; possible values:\n"
   "  ordinary\n"
   "  polynomial\n")
   ("dimension,d", po::value<std::string>(),
    "(required) point set dimension\n")
   ("size-parameter,s", po::value<std::string>(),
    "(required) modulus of the lattice; possible values:\n"
   "  <modulus>\n"
   "  <base>^<max-level>\n"
   "  input format :\n"
   "  ordinary lattice rules: integer (decimal representation)\n"
   "  polynomial lattice rules: polynomial (list of coefficients: 1011 stands for 1 + X^2 + X^3), or\n"
   "                            2^<max_level> (default irreducible modulus, not available for multilevel)\n"
   )
   ("exploration-method,e", po::value<std::string>(),
    "(required) exploration method; possible values:\n"
    "  evaluation:<a1>,...,<as>\n"
    "  exhaustive\n"
    "  random:<r>\n"
    "  Korobov\n"
    "  random-Korobov:<r>\n"
    "  full-CBC\n"
    "  random-CBC:<r>\n"
    "  fast-CBC\n"
    "  extend:<num-points>:<a1>,...,<as>\n"
    "where <r> is the number of samples, "
    "and <a1>,...,<as> are the components of the generating vector\n")
   ("figure-of-merit,f", po::value<std::string>(),
    "(required) type of figure of merit; format: [CU:]<merit>\n"
    "  where the optional \"CU:\" prefix switches on the coordinate-uniform evaluation algorithm,\n"
    "  and where <merit> is one of:\n"
    "    spectral  (only for ordinary lattice rules, no CU)\n"
    "    P<alpha>  (for both constructions, CU for l_2 norm)\n"
    "    R<alpha>  (only for ordinary lattice rules, CU for l_2 norm)\n"
    "    R         (for both constructions, CU for l_2 norm)\n"
    "    IA<alpha> (for interlaced polynomial lattice rules, CU for l_1 norm)\n"
    "    IB (for interlaced polynomial lattice rules, CU for l_1 norm)\n")
    ("interlacing-factor,i", po::value<std::string>()->default_value("1"),
    "(default: 1) interlacing factor (only for polynomial lattice rules). If larger than one, the constructed"
    "point set is an interlaced polynomial lattice rule. In this case, the figure of merit must be"
    "specific to interlaced polynomial lattice rules.\n")
   ("norm-type,q", po::value<std::string>(),
    "norm type used to combine the value of the projection-dependent figure of merit for all projections; possible values:"
    "    <q>: a real number corresponding the l_<q> norm\n"
    "    inf: corresponding to the `max' norm\n")
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
   ("weights-power,p", po::value<Real>(),
    "(default: same value as for the --norm-type argument) real number specifying that the weights passed as input will be assumed to be already elevated at that power (a value of `inf' is mapped to 1)\n")
   ("multilevel,M", po::value<std::string>()->default_value("false"),
    "multilevel point set; possible values:\n"
   "  false (default)\n"
   "  true\n")
   ("combiner,C", po::value<std::string>(),
    "(required for multilevel) combiner for multilevel merit values; possible values:\n"
    "  sum\n"
    "  max\n"
    "  level:{<level>|max}\n")
   ("filters,F", po::value<std::vector<std::string>>()->multitoken(),
    "whitespace-separated list of filters for merit values; possible values:\n"
    "  norm[:<levels>] for a default normalization (if available)"
    "  norm:{P<alpha>-SL10|P<alpha>-DPW08|P<alpha>|IA<alpha>|IB}[:<levels>]\n"
    "  low-pass:<threshold>\n"
    "where in the case of multilevel lattices, the optional parameter <levels> specifies the selected levels; possible values:\n"
    "  select[:<min-level>[:<max-level>]] (default)\n")
   ("repeat,r", po::value<unsigned int>()->default_value(1),
    "(optional) number of times the exploration must be executed\n"
   "(can be useful to obtain different results from random exploration)\n")
   ("verbose,v", po::value<int>()->default_value(0),
   "specify the verbosity of the program;\n"
   "ranges between 0 (default) and 3\n")
    ("output-folder,o", po::value<std::string>(),
    "(optional) path to the folder for the outputs of LatNeBuilder. The contents of the folder may be overwritten. If the folder does not exist, it is created. If no path is provided, no output folder is created.")
    ("output-style,O", po::value<std::string>()->default_value(""),
    "(optional) TBD")
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

   for (const auto x : {"construction", "size-parameter", "exploration-method", "dimension", "figure-of-merit", "norm-type"}) {
      if (opt.count(x) != 1)
         throw std::runtime_error("--" + std::string(x) + " must be specified exactly once (try --help)");
   }

  if (opt.count("combiner") < 1 && opt["multilevel"].as<std::string>() == "true"){
    throw std::runtime_error("--combiner must be specified for multilevel set type (try --help)");
  }

   if (opt.count("weights") < 1)
      throw std::runtime_error("--weights must be specified (try --help)");

   return opt;
}

template <EmbeddingType ET>
std::string helper(const SizeParam<LatticeType::ORDINARY, ET>& param);

template<>
std::string helper(const SizeParam<LatticeType::ORDINARY, EmbeddingType::MULTILEVEL>& param)
{
  return std::to_string(param.base()) + "  // Base\n" + std::to_string(param.maxLevel()) + "  // Maximum level\n";
}

template<>
std::string helper(const SizeParam<LatticeType::ORDINARY, EmbeddingType::UNILEVEL>& param)
{
  return "0  // Base\n0  // Maximum level\n";
}


template <EmbeddingType ET>
std::string helper2(const SizeParam<LatticeType::ORDINARY, ET>& param);

template<>
std::string helper2(const SizeParam<LatticeType::ORDINARY, EmbeddingType::MULTILEVEL>& param)
{
  return ", embedded from " + std::to_string(param.base()) + " to " + std::to_string(param.maxLevel()) + "\n";
}

template<>
std::string helper2(const SizeParam<LatticeType::ORDINARY, EmbeddingType::UNILEVEL>& param)
{
  return "\n";
}



template <EmbeddingType ET>
void executeOrdinary(const Parser::CommandLine<LatticeType::ORDINARY, ET>& cmd, int verbose, unsigned int repeat, std::string outputFolder)
{
   const LatticeType LR = LatticeType::ORDINARY ;
   using namespace std::chrono;

   auto search = cmd.parse();

   const std::string separator = "====================\n";
  
   std::cout << separator << "    Input" << std::endl << separator << *search << std::endl;
    if (outputFolder != ""){
      std::ofstream outFile;
      std::string fileName = outputFolder + "/input.txt";
      outFile.open(fileName);
      outFile << "Input Command Line: " << cmd.originalCommandLine << std::endl << std::endl;
      outFile << *search; 
      outFile.close();
    }

   if (verbose > 0) {
      search->onLatticeSelected().connect(onLatticeSelected<LR,ET>);
      search->setObserverVerbosity(verbose-1);
      search->setVerbose(verbose-2);
   }

   for (unsigned int i = 0; i < repeat; i++) {
        if (repeat > 1){
          std::cout << separator << "      Run " << i+1 << std::endl << separator;
        }
        else{
          std::cout << separator << "Running the task..." << std::endl << separator;
        }

      auto t0 = high_resolution_clock::now();
      search->execute();
      auto t1 = high_resolution_clock::now();

      unsigned int old_precision = (unsigned int) std::cout.precision();
      if (merit_digits_displayed)
   std::cout.precision(merit_digits_displayed);
     const auto lat = search->bestLattice();
     
   auto dt = duration_cast<duration<double>>(t1 - t0);
         std::cout << std::endl;
         std::cout << separator << "      Result" << std::endl << separator;
        std::cout << lat;
        std::cout << "Merit: " << search->bestMeritValue() << std::endl;
        std::cout << std::endl;
         std::cout << "ELAPSED CPU TIME: " << dt.count() << " seconds" << std::endl << std::endl;

      if (outputFolder != ""){
        std::ofstream outFile;
        std::string fileName = outputFolder + "/output.txt";
        outFile.open(fileName);
        outFile << "# Input Command Line: " << cmd.originalCommandLine << std::endl;
        outFile << "# Merit: " << search->bestMeritValue() << std::endl;
        outFile << "# Parameters for a lattice rule";
        outFile << helper2<ET>(lat.sizeParam());
        outFile << lat.dimension() <<"    # "<< lat.dimension() << " dimensions\n";
        outFile << lat.sizeParam().numPoints() <<"    # modulus = n = "<< lat.sizeParam().numPoints() << " points\n";
        auto vec = lat.gen();
        outFile << "# Coordinates of generating vector, starting at j=1" << std::endl;
        for (unsigned int coord = 0; coord < vec.size(); coord++){
          if (coord < vec.size() - 1){
            outFile << vec[coord] << std::endl;
          }
          else{
            outFile << vec[coord];
          }
        }
        outFile.close();
      }
      
      if (merit_digits_displayed)
   std::cout.precision(old_precision);

      search->reset();
    }
}


template <EmbeddingType ET>
void executePolynomial(const Parser::CommandLine<LatticeType::POLYNOMIAL, ET>& cmd, int verbose, unsigned int repeat, std::string outputFolder, NetBuilder::OutputStyle outputStyle)
{
   const LatticeType LR = LatticeType::POLYNOMIAL ;
   using namespace std::chrono;

   auto search = cmd.parse();
   
   unsigned int interlacingFactor = 1;
    try{
      interlacingFactor = boost::lexical_cast<unsigned int>(cmd.interlacingFactor);
    }
    catch (boost::bad_lexical_cast&) {}

   const std::string separator = "====================\n";
  
   std::cout << separator << "    Input" << std::endl << separator << *search << std::endl;
    if (outputFolder != ""){
      std::ofstream outFile;
      std::string fileName = outputFolder + "/input.txt";
      outFile.open(fileName);
      outFile << "Input Command Line: " << cmd.originalCommandLine << std::endl << std::endl;
      outFile << *search;
      outFile.close();
    }

   if (verbose > 0) {
      search->onLatticeSelected().connect(onLatticeSelected<LR,ET>);
      search->setObserverVerbosity(verbose-1);
      search->setVerbose(verbose-2);
   }

   for (unsigned int i = 0; i < repeat; i++) {
        if (repeat > 1){
          std::cout << separator << "      Run " << i+1 << std::endl << separator;
        }
        else{
          std::cout << separator << "Running the task..." << std::endl << separator;
        }

        auto t0 = high_resolution_clock::now();
        search->execute();
        auto t1 = high_resolution_clock::now();

        unsigned int old_precision = (unsigned int) std::cout.precision();
        if (merit_digits_displayed){
          std::cout.precision(merit_digits_displayed);
        }
       const auto lat = search->bestLattice();
      
        auto dt = duration_cast<duration<double>>(t1 - t0);
           std::cout << std::endl;
           std::cout << separator << "    Result" << std::endl << separator;
           std::cout << lat;
           std::cout << "Merit: " << search->bestMeritValue() << std::endl;
           std::cout << std::endl;
           std::cout << "ELAPSED CPU TIME: " << dt.count() << " seconds" << std::endl << std::endl;

      if (outputFolder != ""){
          NetBuilder::DigitalNet<NetBuilder::NetConstruction::POLYNOMIAL> net((unsigned int) lat.gen().size(), lat.sizeParam().modulus(),lat.gen());
          
          if (outputStyle != NetBuilder::OutputStyle::TERMINAL){
            std::ofstream outFile;
            std::string fileName = outputFolder + "/output.txt";
            outFile.open(fileName);
            outFile << "# Input Command Line: " << cmd.originalCommandLine << std::endl;
            outFile << "# Merit: " << search->bestMeritValue() << std::endl;
            outFile << net.format(outputStyle, interlacingFactor) ;
            outFile.close();
          }
      }

        
        if (merit_digits_displayed){
          std::cout.precision(old_precision);
        }
          
        search->reset();
    }
}

   
 




int main(int argc, const char *argv[])
{

   try {
        auto opt = parse(argc, argv);

        // bool quiet = opt.count("quiet");
        int verbose = opt["verbose"].as<int>();
        
        auto repeat = opt["repeat"].as<unsigned int>();

        std::string outputFolder = "";
        if (opt.count("output-folder") >= 1){
          outputFolder = opt["output-folder"].as<std::string>();
          std::cout << "Writing in output folder: " << outputFolder << std::endl;
          boost::filesystem::create_directories(outputFolder);
        }        

        // global variable
        merit_digits_displayed = opt["merit-digits-displayed"].as<unsigned int>();

        std::string outputstyle = opt["output-style"].as<std::string>();

       LatBuilder::LatticeType lattice = Parser::LatticeParser::parse(opt["construction"].as<std::string>());

       std::vector<std::string> all_args;
       if (argc > 1) {
          all_args.assign(argv + 1, argv + argc);
        }

       if(lattice == LatticeType::ORDINARY){

            Parser::CommandLine<LatticeType::ORDINARY, EmbeddingType::MULTILEVEL> cmd;

            
            cmd.originalCommandLine = boost::algorithm::join(all_args, " ");
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

            if (opt["interlacing-factor"].as<std::string>() != "1")
            {
              throw std::runtime_error("Interlacing can only be used with polynomial lattice rules.");
            }
            else
            {
              cmd.interlacingFactor = "1";
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

            EmbeddingType latType = Parser::EmbeddingType::parse(opt["multilevel"].as<std::string>());

            if (latType == EmbeddingType::UNILEVEL){
               executeOrdinary<EmbeddingType::UNILEVEL> (cmd, verbose, repeat, outputFolder);
               
             }
            else{
               executeOrdinary<EmbeddingType::MULTILEVEL> (cmd, verbose, repeat, outputFolder);
               
             }
      }

      else if(lattice == LatticeType::POLYNOMIAL){

            Parser::CommandLine<LatticeType::POLYNOMIAL, EmbeddingType::MULTILEVEL> cmd;

            
            cmd.originalCommandLine = boost::algorithm::join(all_args, " ");
            cmd.construction  = opt["exploration-method"].as<std::string>();
            cmd.size          = opt["size-parameter"].as<std::string>();
            cmd.dimension     = opt["dimension"].as<std::string>();
            cmd.normType      = opt["norm-type"].as<std::string>();
            cmd.figure        = opt["figure-of-merit"].as<std::string>();
            cmd.weights       = opt["weights"].as<std::vector<std::string>>();
            cmd.interlacingFactor = opt["interlacing-factor"].as<std::string>();
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

            EmbeddingType latType = Parser::EmbeddingType::parse(opt["multilevel"].as<std::string>());

            NetBuilder::OutputStyle outputStyle = NetBuilder::Parser::OutputStyleParser<NetBuilder::NetConstruction::POLYNOMIAL>::parse(outputstyle);


            if (latType == EmbeddingType::UNILEVEL){
              executePolynomial< EmbeddingType::UNILEVEL> (cmd, verbose, repeat, outputFolder, outputStyle);
               
             }
            else{
              executePolynomial<EmbeddingType::MULTILEVEL> (cmd, verbose, repeat, outputFolder, outputStyle);
               
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
