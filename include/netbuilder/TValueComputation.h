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

/**
 * \file
 * This file defines several methods to compute the t-value of a digital net in base 2
 */ 

#ifndef NETBUILDER__TVALUE_COMPUTATION_H
#define NETBUILDER__TVALUE_COMPUTATION_H

#include "netbuilder/GeneratingMatrix.h"

namespace NetBuilder {

    /**
     * Class to compute the t-value of a projection of a digital net in base 2.
     * This class uses a refined version of the gaussian elimination to compute efficiently the t-value of
     * a projection, knowing the t-value of the smaller projections. 
     * \todo add a reference if a paper is made.
     */  
    struct GaussMethod
    {

        /**
         * Compute the t-value corresponding to the generating matrices \c baseMatrices, using the prior knowledge that the maximum of the
         * t-values of the subprojections is \c maxTValuesSubProj.
         * @param baseMatrices Generating matrices.
         * @param maxTValuesSubProj Maximum of the t-value of the subprojections.
         * @param verbose Verbosity level.
         */ 
        static unsigned int computeTValue(std::vector<GeneratingMatrix> baseMatrices, unsigned int maxTValuesSubProj, int verbose);

        /**
         * Compute the t-value corresponding to the generating matrices \c baseMatrices, for each level, using the prior knowledge that the maximum of the
         * t-values of the subprojections, for each level \c i is \c maxTValuesSubProj[i].
         * @param baseMatrices Generating matrices.
         * @param maxTValuesSubProj Maximum of the t-value of the subprojections.
         * @param verbose Verbosity level.
         */ 
        static std::vector<unsigned int> computeTValue(std::vector<GeneratingMatrix> baseMatrices, const std::vector<unsigned int>& maxTValuesSubProj, int verbose)
        {
            return computeTValue(baseMatrices, 0, maxTValuesSubProj, verbose);
        };

        /**
         * Compute the t-value corresponding to the generating matrices \c baseMatrices, for each level greater or equal to \c mMin, using the prior knowledge that the maximum of the
         * t-values of the subprojections, for each level <CODE> i + mMin </CODE> is \c maxTValuesSubProj[i]. We do not compute the t-value for the lower levels.
         * @param baseMatrices Generating matrices.
         * @param mMin Minimul level.
         * @param maxTValuesSubProj Maximum of the t-value of the subprojections.
         * @param verbose Verbosity level.
         */ 
        static std::vector<unsigned int> computeTValue(std::vector<GeneratingMatrix> baseMatrices, unsigned int mMin, const std::vector<unsigned int>& maxTValuesSubProj, int verbose);
    };

    /**
     * Class to compute the t-value of a projection of a digital net in base 2.
     * This class uses the algorithm described in \cite rSCH99a, which consists, for each compositions of matrices, in enumerating all the combinations of the rows of the rows in the
     * Gray code order, looking for a linear dependence between the columns.
     */  
    struct SchmidMethod
    {
        /**
         * Compute the t-value corresponding to the generating matrices \c baseMatrices, using the prior knowledge that the maximum of the
         * t-values of the subprojections is \c maxTValuesSubProj.
         * @param baseMatrices Generating matrices.
         * @param maxTValuesSubProj Maximum of the t-value of the subprojections.
         * @param verbose Verbosity level.
         */ 
        static unsigned int computeTValue(std::vector<GeneratingMatrix> baseMatrices, unsigned int maxTValuesSubProj, int verbose);

        /**
         * Compute the t-value corresponding to the generating matrices \c baseMatrices, for each level, using the prior knowledge that the maximum of the
         * t-values of the subprojections, for each level \c i is \c maxTValuesSubProj[i].
         * @param baseMatrices Generating matrices.
         * @param maxTValuesSubProj Maximum of the t-value of the subprojections.
         * @param verbose Verbosity level.
         */ 
        static std::vector<unsigned int> computeTValue(std::vector<GeneratingMatrix> baseMatrices, const std::vector<unsigned int>& maxTValuesSubProj, int verbose);
    };

    /**
     * Class to compute the t-value of a projection of a digital net in base 2.
     * This class uses the algorithm described in \cite rPIR01a.
     */  
    struct PirsicSchmidMethod
    {

        /**
         * Compute the t-value corresponding to the generating matrices \c baseMatrices, using the prior knowledge that the maximum of the
         * t-values of the subprojections is \c maxTValuesSubProj.
         * @param baseMatrices Generating matrices.
         * @param maxTValuesSubProj Maximum of the t-value of the subprojections.
         * @param verbose Verbosity level.
         */ 
        static unsigned int computeTValue(std::vector<GeneratingMatrix> baseMatrices, unsigned int maxTValuesSubProj, int verbose);

        /**
         * Compute the t-value corresponding to the generating matrices \c baseMatrices, for each level, using the prior knowledge that the maximum of the
         * t-values of the subprojections, for each level \c i is \c maxTValuesSubProj[i].
         * @param baseMatrices Generating matrices.
         * @param maxTValuesSubProj Maximum of the t-value of the subprojections.
         * @param verbose Verbosity level.
         */ 
        static std::vector<unsigned int> computeTValue(std::vector<GeneratingMatrix> baseMatrices, const std::vector<unsigned int>& maxTValuesSubProj, int verbose)
        {
            return computeTValue(baseMatrices, 0, maxTValuesSubProj, verbose);
        };

        /**
         * Compute the t-value corresponding to the generating matrices \c baseMatrices, for each level greater or equal to \c mMin, using the prior knowledge that the maximum of the
         * t-values of the subprojections, for each level <CODE> i + mMin </CODE> is \c maxTValuesSubProj[i]. We do not compute the t-value for the lower levels.
         * @param baseMatrices Generating matrices.
         * @param mMin Minimul level.
         * @param maxTValuesSubProj Maximum of the t-value of the subprojections.
         * @param verbose Verbosity level.
         */ 
        static std::vector<unsigned int> computeTValue(std::vector<GeneratingMatrix> baseMatrices, unsigned int mMin, const std::vector<unsigned int>& maxTValuesSubProj, int verbose);
    };

}

#endif


