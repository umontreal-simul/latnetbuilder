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

#ifndef NETBUILDER__TASK__EVAL_H
#define NETBUILDER__TASK__EVAL_H

#include "netbuilder/Types.h"

#include "netbuilder/Task/Task.h"
#include "netbuilder/FigureOfMerit/FigureOfMerit.h"

#include <boost/signals2.hpp>

#include <memory>
#include <limits>

namespace NetBuilder { namespace Task {

class Eval : public Task 
{
    public:

        Eval(std::unique_ptr<DigitalNet> net, std::unique_ptr<FigureOfMerit::FigureOfMerit> figure, int verbose = 0):
            m_net(std::move(net)),
            m_merit(0),
            m_figure(std::move(figure)),
            m_verbose(verbose)
        {};

        Eval(Eval&&) = default;

        ~Eval() = default;

        /**
        * Returns the dimension.
        */
        Dimension dimension() const
        { return m_net->dimension(); }

        unsigned int nRows() {return m_net->numRows(); }

        unsigned int nCols() {return m_net->numColumns(); }

        /**
        * Returns the best net found by the search task.
        */
        const DigitalNet& net() const
        { return *m_net; }

        /**
        * Returns the best net found by the search task.
        */
        virtual std::string outputNet(OutputFormat outputFormat, OutputMachineFormat outputMachineFormat, unsigned int interlacingFactor) const 
        { return net().format(outputFormat, outputMachineFormat, interlacingFactor); }

        /**
         *  Returns information about the task
         */
        virtual std::string format() const 
        {
            std::string res;
            std::ostringstream stream;
            stream << "Task: NetBuilder Evaluation" << std::endl;
            stream << "Number of components: " << this->dimension() << std::endl;
            stream << "Evaluation of the net:" << std::endl;
            stream << m_net->format(OutputFormat::HUMAN, OutputMachineFormat::NONE, 1);
            stream << "Figure of merit: " << m_figure->format() << std::endl;
            res += stream.str();
            stream.str(std::string());
            return res;
        }

        /**
        * Returns the best merit value found by the search task.
        */
        Real meritValue() const
        { return m_merit; }

        /**
        * Returns the best merit value found by the search task.
        */
        virtual Real outputMeritValue() const 
        { return meritValue(); }

        const FigureOfMerit::FigureOfMerit& figureOfMerit() const 
        {
               return *m_figure;
        }

        /**
        * Executes the search task.
        *
        * The best net and merit value are set in the process.
        */
        virtual void execute() {

            auto evaluator = m_figure->evaluator(); 
            m_merit = evaluator->operator()(*m_net, m_verbose);
        }

        virtual void reset()
        {
            m_merit = 0;
        }

        virtual void reset(std::unique_ptr<DigitalNet> net)
        {
            m_net = std::move(net);
            m_merit = 0;
        }

    private:

        std::unique_ptr<DigitalNet> m_net;
        Real m_merit;
        std::unique_ptr<FigureOfMerit::FigureOfMerit> m_figure;
        int m_verbose;

};


}}

#endif
