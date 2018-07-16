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

#ifndef NET_BUILDER__FIGURE_OF_MERIT__TVALUE_H
#define NET_BUILDER__FIGURE_OF_MERIT__TVALUE_H

#include "netbuilder/FigureOfMerit/FigureOfMerit.h"
#include "latbuilder/Storage.h"

namespace NetBuilder { namespace FigureOfMerit {

/**
 * Class to represent figure of merit based on the t-value of a net.
 * The computation if the t-value uses Algorithm 1 proposed by
 * Dick and Matsumoto in \cite rDIC13a.
 * The multilevel computation of the t-value is based on the second
 * item of Remark 3.
 * Note that this algorithm has strong similiarities with
 * the coordinate-uniform evaluation algorithm used for kernel-based
 * figures. That is why the Storage class from LatBuilder is used here,
 * with the same trick as for CoordUniformFigureOfMerit.
 */
template <EmbeddingType ET>
class TValue : public FigureOfMerit
{
    public:

        /**
         * Constructor.
         * @param combiner Combiner  multilevel merits. Only used when template parameter \c ET
         * equals <code>EmbeddingType::MULTILEVEL</code>.
         */ 
        TValue(Combiner combiner = Combiner()):
            m_combiner(std::move(combiner))
        {};

        std::unique_ptr<FigureOfMeritEvaluator> createEvaluator() override
        {
            return std::make_unique<TValueEvaluator>(this);
        }

        MeritValue combine(const RealVector& merits)
        {
            return m_combiner(merits);
        }


    private:
        /**
         * Evaluator for the t-value figure of merit.
         */ 
        class TValueEvaluator:
            public FigureOfMeritEvaluator
        {
            public:
            
                /// Storage class 
                typedef LatBuilder::Storage<LatBuilder::LatticeType::DIGITAL, ET,  LatBuilder::Compress::NONE> Storage;

                /**
                 * Constructor.
                 * @param figure Pointer to the figure of merit.
                 */ 
                TValueEvaluator(TValue* figure):
                    m_dimension(0),
                    m_numLevels(0),
                    m_figure(figure),
                    m_kernelValues(0)
                {
                    updateDimensionAndNbLevels(1, 1);
                };

                /** 
                 * Computes the figure of merit for the given \c net for all the dimensions (full computation).
                 * @param net Net to evaluate.
                 * @param verbose Verbosity level.
                 */ 
                virtual MeritValue operator() (const DigitalNet& net, int verbose = 0) override
                {
                    Dimension s = net.dimension();
                    unsigned int k = net.numColumns();

                    updateDimensionAndNbLevels(s, k);  // update the pre-computed quantities according to dimension and size of the net

                    std::vector<boost::numeric::ublas::vector_indirect<const boost::numeric::ublas::vector<uInteger> , typename LatBuilder::IndexMap< typename LatBuilder::StorageTraits<Storage>::Stride>>> permutedValues;
                    permutedValues.reserve(s);

                    // compute the permutations of the kernel values for all coordinates
                    for(Dimension coord = 0; coord < s; ++coord)
                    {
                        permutedValues.push_back(m_storage->strided(kernelValues(), net.generatingMatrix(coord)));
                    }

                    IntPolynomial truncWeightPoly(0);
                    for(size_t i = 0; i < (1 << k); ++i)
                    {
                        IntPolynomial prod(1);
                        for(Dimension coord = 0; coord < s; ++coord)
                        {
                            IntPolynomial fact(1);
                            NTL::SetCoeff(fact, permutedValues[coord][i], - (1 << permutedValues[coord][i]));
                            prod = NTL::MulTrunc(prod, fact, k + 1);
                        }
                        truncWeightPoly += prod;
                    }
                    truncWeightPoly = NTL::MulTrunc(m_auxPoly, truncWeightPoly, k + 1);
                    unsigned int rho = 1;
                    while(rho <= k && NTL::coeff(truncWeightPoly, rho) == 0)
                    {
                        ++rho;
                    }
                    return k + 1 - rho;
                }

                /**     
                 * Resets the evaluator and prepare it to evaluate a new net.
                 */ 
                virtual void reset() override
                {
                    return;
                }

            private:

                Dimension m_dimension;
                unsigned int m_numLevels;
                IntPolynomial m_auxPoly;
                TValue* m_figure;
                std::unique_ptr<Storage> m_storage; // storage for the kernel values
                boost::numeric::ublas::vector<uInteger> m_kernelValues;

                typedef LatBuilder::SizeParam<LatBuilder::LatticeType::DIGITAL, ET> SizeParam;

                /**
                 * Updates the pre-computed quantities used by the algorithm if required.
                 */ 
                void updateDimensionAndNbLevels(Dimension s, unsigned int m)
                {
                    if (s != m_dimension || m != m_numLevels)
                    {
                        m_dimension = s;
                        m_numLevels = m;
                        m_storage = std::make_unique<Storage>(SizeParam(1 << m_numLevels));
                        updateAuxPoly();
                        updateKernelValues();
                    }
                }

                /**
                 * Recomputes the auxiliary polynomial \f$Q_m(z)\f$
                 */ 
                void updateAuxPoly()
                {
                    m_auxPoly = IntPolynomial(1);
                    IntPolynomial base(1);
                    for (unsigned int j = 1; j <= m_numLevels; ++j)
                    {
                        NTL::SetCoeff(base, j, 1 << (j - 1)) ;
                    }
                    for (unsigned int k = 1; k <= m_dimension; ++k)
                    {
                        m_auxPoly = NTL::MulTrunc(base, m_auxPoly, m_numLevels + 1);
                    }
                }

                /**
                 * Recomputes the kernel values \f$\nu^\star(\frac{i}{2^k})\f$, \f$ i = 0, \dots, 2^k-1\f$.
                 */ 
                void updateKernelValues()
                {
                    m_kernelValues.resize(1 << m_numLevels);
                    m_kernelValues[0] = m_numLevels + 1;
                    uInteger width = 1;
                    uInteger index = 1;
                    for(unsigned int value = m_numLevels; value >= 1; --value)
                    {
                        for(uInteger i = index; i < index+width; ++i)
                        {
                            m_kernelValues[i] = value;
                        }
                        index = index + width;
                        width *= 2;
                    }
                }

                /**
                 * Returns a const reference to the kernel values
                 */ 
                const boost::numeric::ublas::vector<uInteger>& kernelValues() const
                {
                    return m_kernelValues;
                }

        };

        Combiner m_combiner;
};

/**
 * Templace specialization of <code>operator()</code> of TValueEvaluator in the multilevel case.
 */ 
template<>
MeritValue TValue<EmbeddingType::MULTILEVEL>::TValueEvaluator::operator()(const DigitalNet& net, int verbose)
{
    Dimension s = net.dimension();
    unsigned int k = net.numColumns();

    updateDimensionAndNbLevels(s, k); 

    std::vector<boost::numeric::ublas::vector_indirect<const boost::numeric::ublas::vector<uInteger> , typename LatBuilder::IndexMap< typename LatBuilder::StorageTraits<Storage>::Stride>>> permutedValues;
    permutedValues.reserve(s);

    for(Dimension coord = 0; coord < s; ++coord)
    {
        permutedValues.push_back(m_storage->strided(kernelValues(), net.generatingMatrix(coord)));
    }

    RealVector merits(k);

    IntPolynomial truncWeightPoly(0);
    NTL::SetCoeff(truncWeightPoly, 0, 1);
    size_t i = 1;
    for(unsigned int m = 1; m <= k; ++m)
    {
        for(size_t j = i; j < 2 * i ; ++j)
        {
            IntPolynomial prod(1);
            for(Dimension coord = 0; coord < s; ++coord)
            {
                IntPolynomial fact(1);
                NTL::SetCoeff(fact, permutedValues[coord][j], - (1 << permutedValues[coord][j]));
                prod = NTL::MulTrunc(prod, fact, k + 1);
            }
            truncWeightPoly += prod;
        }
        IntPolynomial partialTruncWeightPoly = NTL::MulTrunc(m_auxPoly, truncWeightPoly, m + 1);
        unsigned int rho = 1;
        while(rho <= m && NTL::coeff(partialTruncWeightPoly, rho) == 0)
        {
            ++rho;
        }
        merits[m-1] = (m + 1 - rho);
        i *= 2;
    }
    return m_figure->combine(merits);
}

}}

#endif
