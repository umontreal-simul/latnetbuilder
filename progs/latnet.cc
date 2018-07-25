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

#include <boost/program_options.hpp>
#include <iostream>

#include "netbuilder/Path.h"
#include "latbuilder/LatBuilder.h"
#include "netbuilder/NetBuilder.h"

#ifndef LATNETBUILDER_VERSION
#define LATNETBUILDER_VERSION "(unkown version)"
#endif

boost::program_options::options_description
makeOptionsDescription()
{
    namespace po = boost::program_options;
    po::options_description desc("allowed options");

    desc.add_options()
    ("help,h", "produce help message")
    ("version,V", "show version")
    ("set-type,t", po::value<std::string>(),
        "(required) point set type; possible values:\n"
        "  lattice\n"
        "  net\n");

    return desc;
}

int main(int argc, const char *argv[])
{
    try
    {
        NetBuilder::SET_PATH_TO_LATNETBUILDER_DIR_FROM_PROGRAM_NAME(argv[0]);
        namespace po = boost::program_options;

        auto desc = makeOptionsDescription();

        po::variables_map opt;
        // po::store(po::parse_command_line(argc, argv, desc), opt);
        po::store(po::command_line_parser(argc, argv).options(desc).allow_unregistered().run(), opt);
        po::notify(opt);

        if (opt.count("help") && opt.count("set-type") < 1)
        {
            std::cout << desc << std::endl;
            std::exit(0);
        }

        if (opt.count("version"))
        {
            std::cout << "LatNet Builder " << LATNETBUILDER_VERSION << std::endl;
            std::exit(0);
        }

        if (opt.count("set-type") < 1)
        {
            throw std::runtime_error("point set type must be specified; see --help");
        }
        if (opt["set-type"].as<std::string>() == "lattice")
        {
            LatBuilder::main(argc, argv);
        }
        else if (opt["set-type"].as<std::string>() == "net")
        {
            NetBuilder::main(argc, argv);
        }
        else
        {
            throw std::runtime_error("point set type not recognized; see --help");
        }
    }
    catch (std::exception& e) {
      std::cerr << "ERROR: " << e.what() << std::endl;
      std::exit(1);
    }

   return 0;
}