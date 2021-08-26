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


#include <fstream>
#include <chrono>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/join.hpp>
#include <iostream>
#include <limits>

#include "netbuilder/Types.h"
#include "netbuilder/Parser/CommandLine.h"
#include "netbuilder/Parser/EmbeddingTypeParser.h"
#include "netbuilder/Parser/NetConstructionParser.h"
#include "netbuilder/Parser/OutputStyleParser.h"
#include "netbuilder/Task/Task.h"

#include "latbuilder/Parser/Common.h"
#include "latbuilder/SizeParam.h"

// using namespace LatBuilder;
// using TextStream::operator<<;

namespace NetBuilder{
static unsigned int merit_digits_displayed = 0;

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
   "digital net; possible constructions:\n"
   "  sobol\n"
   "  polynomial\n"
   "  lms\n"
   "  explicit\n")
    ("dimension,d", po::value<std::string>(),
    "(required) point set dimension\n")
   ("size-parameter,s", po::value<std::string>(),
    "(required) modulus of the net; possible values:\n"
   "  2^<max-level>\n"
   "  <modulus>: polynomial (list of coefficients: 1011 stands for 1 + X^2 + X^3) (only for polynomial construction)\n"
   )
   ("exploration-method,e", po::value<std::string>(),
    "(required) exploration method; possible values:\n"
    "  evaluation:<net_description>\n" 
    "  exhaustive\n"
    "  random:<r>\n"
    "  full-CBC\n"
    "  random-CBC:<r>\n"
    "  mixed-CBC:<r>:<nb_full>\n"
    "where <net_description> is a net description (see documentation), <r> is the number of samples, and <nb_full> the number of coordinates for which full CBC exploration is used.")
   ("figure-of-merit,f", po::value<std::string>(),
    "(required) type of figure of merit; format: <merit>\n"
    "  and where <merit> is one of:\n"
    "    CU:P<alpha>  (requires l_2 norm)\n"
    "    CU:R (requires l_2 norm)\n"
    "    t-value (weights and norm-type are ignored)\n"
    "    projdep:t-value\n"
    "    projdep:t-value:starDisc\n"
    "    projdep:resolution-gap\n"
    "    CU:IA<alpha> (only for interlaced digital nets, requires l_1 norm)\n"
    "    CU:IB (only for interlaced digital nets, requires l_1 norm)\n"
    "where the \"CU:\" prefix stands for the coordinate-uniform evaluation algorithm and projdep for projection-dependent figure without coordinate-uniform evaluation algorithm\n")
    ("interlacing-factor,i", po::value<unsigned int>()->default_value(1),
    "(default: 1) interlacing factor. If larger than one, the constructed "
    "point set is an interlaced digital net. In this case, the figure of merit must be"
    "specific to interlaced digital nets.\n")
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
   ("repeat,r", po::value<unsigned int>()->default_value(1),
    "(optional) number of times the construction must be executed\n"
   "(can be useful to obtain different results from random constructions)\n")
    ("verbose,v", po::value<std::string>()->default_value("0"),
   "specify the verbosity of the program;\n"
   "ranges between 0 (default) and 3\n")
    ("output-folder,o", po::value<std::string>(),
    "(optional) path to the folder for the outputs of LatNeBuilder. The contents of the folder may be overwritten. If the folder does not exist, it is created. If no path is provided, no output folder is created.")
    ("output-style,O", po::value<std::string>()->default_value(""),
    "(optional) TBD\n")
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

   if (opt.count("weights") < 1)
      throw std::runtime_error("--weights must be specified (try --help)");
   for (const auto x : {"size-parameter", "exploration-method", "dimension", "figure-of-merit", "norm-type"}) {
      if (opt.count(x) != 1)
         throw std::runtime_error("--" + std::string(x) + " must be specified exactly once (try --help)");
   }

    if (opt["multilevel"].as<std::string>() == "true" && ! opt.count("combiner")){
      throw std::runtime_error("--combiner must be specified for multilevel set type (try --help)");
    }
   return opt;
}


#define BUILD_TASK(net_construction, point_set_type)\
NetBuilder::Parser::CommandLine<NetBuilder::NetConstruction::net_construction, NetBuilder::EmbeddingType::point_set_type> cmd;\
\
cmd.s_verbose = opt["verbose"].as<std::string>();\
cmd.s_explorationMethod = opt["exploration-method"].as<std::string>();\
cmd.s_size = opt["size-parameter"].as<std::string>();\
cmd.s_dimension = opt["dimension"].as<std::string>();\
cmd.s_figure = opt["figure-of-merit"].as<std::string>();\
cmd.s_weights       = opt["weights"].as<std::vector<std::string>>();\
cmd.m_normType = boost::lexical_cast<Real>(opt["norm-type"].as<std::string>());\
cmd.m_interlacingFactor = opt["interlacing-factor"].as<unsigned int>(); \
interlacingFactor = cmd.m_interlacingFactor;\
if (opt.count("combiner") < 1){\
  cmd.s_combiner = "";\
}\
else{\
  cmd.s_combiner = opt["combiner"].as<std::string>();\
}\
if (opt.count("weight-power") == 1 ){\
  cmd.m_weightPower = boost::lexical_cast<Real>(opt["weight-power"].as<std::string>());\
}\
else{\
  if (cmd.m_normType < std::numeric_limits<Real>::infinity())\
  {\
    cmd.m_weightPower = cmd.m_normType;\
  }\
  else\
  {\
    cmd.m_weightPower = 1;\
  }\
}\
task = cmd.parse();\
outputStyle = NetBuilder::Parser::OutputStyleParser<NetBuilder::NetConstruction::net_construction>::parse(s_outputStyle);


void TaskOutput(const Task::Task &task, std::string outputFolder, OutputStyle outputStyle, unsigned int interlacingFactor, std::vector<std::string> inputCL)
{
  unsigned int old_precision = (unsigned int)std::cout.precision();
  if (merit_digits_displayed){
    std::cout.precision(merit_digits_displayed);
  }
  std::cout << "====================\n       Result\n====================" << std::endl;
  std::cout << task.outputNet(OutputStyle::TERMINAL, interlacingFactor) << "Merit: " << task.outputMeritValue() << std::endl;

  if (outputFolder != ""){
    std::ofstream outFile;
    std::string fileName = outputFolder + "/output.txt";
    outFile.open(fileName);
    outFile << "# Input Command Line: " << boost::algorithm::join(inputCL, " ") << std::endl;
    outFile << "# Merit: " << task.outputMeritValue() << std::endl;
    outFile << task.outputNet(outputStyle, interlacingFactor);
    outFile.close();
  }
  
  if (merit_digits_displayed){
    std::cout.precision(old_precision);
  }
}


int main(int argc, const char *argv[])
{

   try {
     using namespace std::chrono;
     const std::string separator = "\n--------------------------------------------------------------------------------\n";


        auto opt = parse(argc, argv);

        auto repeat = opt["repeat"].as<unsigned int>();

        std::string outputFolder = "";
        if (opt.count("output-folder") >= 1){
          outputFolder = opt["output-folder"].as<std::string>();
          std::cout << "Writing in output folder: " << outputFolder << std::endl;
          if (! boost::filesystem::is_directory(outputFolder)){
            boost::filesystem::create_directories(outputFolder);
          }
        }
        
        // global variable
        merit_digits_displayed = opt["merit-digits-displayed"].as<unsigned int>();

        std::string s_multilevel = opt["multilevel"].as<std::string>();
        std::string s_construction = opt["construction"].as<std::string>();
        std::string s_outputStyle = opt["output-style"].as<std::string>();
        NetBuilder::EmbeddingType embeddingType = NetBuilder::Parser::EmbeddingTypeParser::parse(s_multilevel);

        NetBuilder::NetConstruction netConstruction;

        if (embeddingType==NetBuilder::EmbeddingType::UNILEVEL)
        {
          netConstruction =  NetBuilder::Parser::NetConstructionParser<NetBuilder::EmbeddingType::UNILEVEL>::parse(s_construction);
        }
        else
        {
          netConstruction =  NetBuilder::Parser::NetConstructionParser<NetBuilder::EmbeddingType::MULTILEVEL>::parse(s_construction);
        }

        std::chrono::time_point<std::chrono::high_resolution_clock> t0, t1;
        unsigned int interlacingFactor = 0;
      
        std::unique_ptr<NetBuilder::Task::Task> task;
        NetBuilder::OutputStyle outputStyle;

       if(netConstruction == NetBuilder::NetConstruction::SOBOL && embeddingType == NetBuilder::EmbeddingType::UNILEVEL){
          BUILD_TASK(SOBOL, UNILEVEL)
       }
       else if(netConstruction == NetBuilder::NetConstruction::SOBOL && embeddingType == NetBuilder::EmbeddingType::MULTILEVEL){
          BUILD_TASK(SOBOL, MULTILEVEL)
       }
       else if(netConstruction == NetBuilder::NetConstruction::POLYNOMIAL && embeddingType == NetBuilder::EmbeddingType::UNILEVEL){
          BUILD_TASK(POLYNOMIAL, UNILEVEL)
       }
       else if(netConstruction == NetBuilder::NetConstruction::EXPLICIT && embeddingType == NetBuilder::EmbeddingType::UNILEVEL){
          BUILD_TASK(EXPLICIT, UNILEVEL)
       }
       else if(netConstruction == NetBuilder::NetConstruction::EXPLICIT && embeddingType == NetBuilder::EmbeddingType::MULTILEVEL){
          BUILD_TASK(EXPLICIT, MULTILEVEL)
       }
       else if(netConstruction == NetBuilder::NetConstruction::LMS && embeddingType == NetBuilder::EmbeddingType::UNILEVEL){
          BUILD_TASK(LMS, UNILEVEL)
       }
       else if(netConstruction == NetBuilder::NetConstruction::LMS && embeddingType == NetBuilder::EmbeddingType::MULTILEVEL){
          BUILD_TASK(LMS, MULTILEVEL)
       }
       else {
         throw std::runtime_error("Unknown combination of NetConstruction and EmbeddingType");
       }

      std::vector<std::string> inputCL;
      if (argc > 1) {
        inputCL.assign(argv + 1, argv + argc);
      }


      for (unsigned i=0; i<repeat; i++){
        if (i == 0){
          std::cout << "====================\n       Input\n====================" << std::endl;
          std::cout << task->format();
          std::cout << std::endl;

          if (outputFolder != ""){
            std::ofstream outFile;
            std::string fileName = outputFolder + "/input.txt";
            outFile.open(fileName);
            outFile << "Input Command Line: " << boost::algorithm::join(inputCL, " ") << std::endl << std::endl;
            outFile << task->format();
            outFile.close();
          }
        }

          if (repeat > 1){
            std::cout << "====================\n       Run " << i+1 << "\n====================" << std::endl;
          }
          else{
            std::cout << "====================\nRunning the task... \n====================" << std::endl;
          }

          t0 = high_resolution_clock::now();
          task->execute();
          t1 = high_resolution_clock::now();
          auto dt = duration_cast<duration<double>>(t1 - t0);

          std::cout << std::endl;
          TaskOutput(*task, outputFolder, outputStyle, interlacingFactor, inputCL);
          std::cout << std::endl;
          std::cout << "ELAPSED CPU TIME: " << dt.count() << " seconds" << std::endl;
          task->reset();
      }
   }
   catch (LatBuilder::Parser::ParserError& e) {
      std::cerr << "COMMAND LINE ERROR: " << e.what() << std::endl;
      std::exit(1);
   }
   catch (std::exception& e) {
      std::cerr << "ERROR: " << e.what() << std::endl;
      std::exit(1);
   }

   return 0;
}

#undef BUILD_TASK
}
