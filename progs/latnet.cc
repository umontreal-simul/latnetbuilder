// This file is part of Lattice Builder.
//
// Copyright (C) 2012-2016  Pierre L'Ecuyer and Universite de Montreal
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

#include <boost/program_options.hpp>
#include <iostream>

#include "latbuilder/LatBuilder.h"
#include "netbuilder/NetBuilder.h"

#ifndef LATBUILDER_VERSION
#define LATBUILDER_VERSION "(unkown version)"
#endif

boost::program_options::options_description
makeOptionsDescription()
{
    namespace po = boost::program_options;
    po::options_description desc("allowed options");

    desc.add_options()
    ("help,h", "produce help message")
    ("version,V", "show version")
    ("main-construction,C", po::value<std::string>(),
        "(required) main construction type; possible values:\n"
        "  lattice\n"
        "  net\n");

    return desc;
}

int main(int argc, const char *argv[])
{
    try
    {
        namespace po = boost::program_options;

        auto desc = makeOptionsDescription();

        po::variables_map opt;
        // po::store(po::parse_command_line(argc, argv, desc), opt);
        po::store(po::command_line_parser(argc, argv).options(desc).allow_unregistered().run(), opt);
        po::notify(opt);

        if (opt.count("main-construction") < 1 && opt.count("help"))
        {
            std::cout << desc << std::endl;
            std::exit(0);
        }

        if (opt.count("version"))
        {
            std::cout << "Lattice Builder " << LATBUILDER_VERSION << std::endl;
            std::exit(0);
        }

        if (opt.count("main-construction") < 1)
        {
            throw std::runtime_error("main construction type must be specified; see --help");
        }
        if (opt["main-construction"].as<std::string>() == "lattice")
        {
            LatBuilder::main(argc, argv);
        }
        else if (opt["main-construction"].as<std::string>() == "net")
        {
            NetBuilder::main(argc, argv);
        }
        else
        {
            throw std::runtime_error("main construction type not recognized; see --help");
        }
    }
    catch (std::exception& e) {
      std::cerr << "ERROR: " << e.what() << std::endl;
      std::exit(1);
    }

   return 0;
}