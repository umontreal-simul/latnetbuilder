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


#include "netbuilder/Types.h"
#include "netbuilder/DigitalNet.h"
#include "netbuilder/Task/MinimumObserver.h"
#include "netbuilder/Task/RandomSearch.h"

#include "netbuilder/FigureOfMerit/CoordUniformFigureOfMerit.h"
#include "latbuilder/Kernel/PAlphaPLR.h"
#include "latticetester/ProductWeights.h"

#include "latbuilder/TextStream.h"
using LatBuilder::TextStream::operator<<;

#include <boost/ref.hpp>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/count.hpp>
#include <boost/accumulators/statistics/min.hpp>
#include <boost/accumulators/statistics/max.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/tail_quantile.hpp>

using namespace NetBuilder;
using namespace boost::accumulators;

//! [observer]
template <NetConstruction NC>
class QuantilesObserver : public Task::MinimumObserver<NC>
{
    public:

        typedef accumulator_set<Real, features<tag::count, tag::min, tag::max, tag::mean, tag::tail_quantile<boost::accumulators::left>>> QuantilesAccumulator;

        QuantilesObserver(typename NetConstructionTraits<NC>::SizeParameter sizeParameter, int verbose = 0):
            Task::MinimumObserver<NC>(sizeParameter, verbose),
            m_numSamples(0),
            m_acc(tag::tail<boost::accumulators::left>::cache_size = m_numSamples)
        {};
            
        virtual void reset(bool hard = true) override
        { 
            Task::MinimumObserver<NC>::reset(hard);
            if (hard)
                m_acc = QuantilesAccumulator(tag::tail<boost::accumulators::left>::cache_size = 0);
        }

        void setNumSamples(size_t numSamples)
        {
            m_numSamples = numSamples;
            m_acc = QuantilesAccumulator(tag::tail<boost::accumulators::left>::cache_size = m_numSamples);
        }

        virtual bool observe(std::unique_ptr<DigitalNetConstruction<NC>> net, const Real& merit) override
        {
            m_acc(merit);
            return Task::MinimumObserver<NC>::observe(std::move(net), merit);
        }

        const QuantilesAccumulator& accumulator()
        {
            return m_acc;
        }

        private:
            size_t m_numSamples;
            QuantilesAccumulator m_acc;
};
//! [observer]

template <typename T1, typename T2>
void printTableRow(const T1& x1, const T2& x2)
{
   using namespace std;
   cout << fixed << setprecision(3) << x1 << '\t'
      << scientific << setprecision(8) << x2 << std::endl;
}

int main(int argc, const char *argv[])
{
    //! [search_params]
    size_t numSamples = 1000;
    Dimension s = 10;
    NetConstructionTraits<NetConstruction::EXPLICIT>::SizeParameter sizeParam(10, 10);
    unsigned int alpha = 2;
    auto kernel = LatBuilder::Kernel::PAlphaPLR(alpha);
    auto weights = std::make_unique<LatticeTester::ProductWeights>(.7);
    auto figure = std::make_unique<FigureOfMerit::CoordUniformFigureOfMerit<LatBuilder::Kernel::PAlphaPLR, EmbeddingType::UNILEVEL>>(std::move(weights), kernel);
    std::cout << figure->format() << std::endl;
    //! [search_params]

    //! [task]
    auto task = Task::RandomSearch<NetConstruction::EXPLICIT, EmbeddingType::UNILEVEL, QuantilesObserver>(s, sizeParam, std::move(figure), (unsigned int) numSamples);
    task.observer().setNumSamples(numSamples);
    task.execute();
    //! [task]

    //! [output]
    const auto& acc = task.observer().accumulator();
    unsigned int numBins = 20;
    printTableRow("# mean:", mean(acc));
    printTableRow("prob", "quantile");
    printTableRow(0.0, boost::accumulators::min(acc));
    for (unsigned int i = 1; i < numBins; i++) {
        double p = double(i) / numBins;
        Real q = quantile(acc, quantile_probability = p);
        printTableRow(p, q);
    }
    printTableRow(1.0, boost::accumulators::max(acc));
    //! [output]

    return 0;
}