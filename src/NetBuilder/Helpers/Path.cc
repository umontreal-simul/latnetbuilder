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

#include "netbuilder/Helpers/Path.h"
#include <boost/filesystem.hpp>
#include <cstdio>
#include <array>
#include <stdexcept>

namespace NetBuilder{

    static std::string exec(const char* cmd) {
        std::array<char, 128> buffer;
        std::string result;
        std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
        if (!pipe) throw std::runtime_error("popen() failed!");
        while (!feof(pipe.get())) {
            if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
                result += buffer.data();
        }
        return result;
    }

    std::string PATH_TO_LATNETBUILDER_DIR = ".";

    void SET_PATH_TO_LATNETBUILDER_DIR_FROM_PROGRAM_NAME(const char* argv0)
    {
        if (argv0 == nullptr)
        {
            FIND_PATH_TO_LATNETBUILDER_DIR();
            return;
        }
        boost::filesystem::path p(argv0);
        std::string path = p.parent_path().string();
        if (path == ""){
            FIND_PATH_TO_LATNETBUILDER_DIR();
            return;
        }
        SET_PATH_TO_LATNETBUILDER_DIR(path);
    }

    void FIND_PATH_TO_LATNETBUILDER_DIR()
    {
        std::string path = exec("which latnetbuilder");
        boost::filesystem::path p(path);
        SET_PATH_TO_LATNETBUILDER_DIR(p.parent_path().string());
    }

    void SET_PATH_TO_LATNETBUILDER_DIR(const std::string& path)
    {
        PATH_TO_LATNETBUILDER_DIR = path;
        CHECK_PATH_TO_LATNETBUILDER_DIR();
    }

    void CHECK_PATH_TO_LATNETBUILDER_DIR()
    {
        if(!boost::filesystem::exists(PATH_TO_LATNETBUILDER_DIR+ "/latnetbuilder" ))
        {  
            throw std::runtime_error("The path to the latnetbuilder executable is not correctly set.");
        }
        if(!boost::filesystem::exists(PATH_TO_LATNETBUILDER_DIR+ "/../share/latnetbuilder/data" ))
        {  
            throw std::runtime_error("The path to the latnetbuilder executable is not correctly set. 'share/latnebuilder/data' is missing");
        }
    }

}

