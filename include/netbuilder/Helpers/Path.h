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


#ifndef NETBUILDER__PATH_H
#define NETBUILDER__PATH_H

#include <string>
/**
 * \file 
 * This file contains a global variable PATH_TO_LATNETBUILDER_DIR which should always equal
 * the path to the directory in which the latnetbuilder executable is located.
 * From this directory, the library recovers the location of the data files required for
 * some functionalities.
 */

namespace NetBuilder{

    /// Path to the directory containing the latnetbuilder executable
    extern std::string PATH_TO_LATNETBUILDER_DIR;

    /** Sets the path to the directory containing the latnetbuilder executable. 
     *  Should only be call from an executable which is located in the same 
     *  directory as the latnetbuilder executable.
     *  @param argv0 argv[0] from the main function arguments.
     */ 
    void SET_PATH_TO_LATNETBUILDER_DIR_FROM_PROGRAM_NAME(const char* argv0);

    /**
     * Recovers the path
     * to the latnetbuilder executable if it was added
     * to the PATH environment variable.
     */ 
    void FIND_PATH_TO_LATNETBUILDER_DIR();

    /**
     * Sets the path to the directory in which the latnetbuilder executable
     * is located.
     * \c path can be a relative path from the executable directory
     * (in this case when the executable is called the current
     *  working directory must match the executable location).
     * Alternatively, \c path can be a relative path from
     * the working directory from which the executable will be called.
     * Finally, \c path can be an absolute path.
     * @param path New value of PATH_TO_LATNETBUILDER_DIR.
     */ 
    void SET_PATH_TO_LATNETBUILDER_DIR(const std::string& path);


    /**
     * Checks if the PATH_TO_LATNETBUILDER_DIR is correctly set:
     * existence of the latnetbuilder executable in the directory
     * and existence of the ../share/latnetbuilder/data and
     * ../share/latticetester/data directories.
     */ 
    void CHECK_PATH_TO_LATNETBUILDER_DIR();
}

#endif