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

#ifndef NETBUILDER__TASK__SEARCH_H
#define NETBUILDER__TASK__SEARCH_H

#include "netbuilder/Task/Task.h"
#include "netbuilder/Task/MinimumObserver.h"

#include "netbuilder/FigureOfMerit/FigureOfMerit.h"

#include <ostream>
#include <memory>

namespace NetBuilder { namespace Task {


namespace{
    template <NetConstruction NC> 
    std::string output(typename NetConstructionTraits<NC>::SizeParameter sizeParameter)
    {
        return "";
    };

    template<>
    std::string output<NetConstruction::POLYNOMIAL>(typename NetConstructionTraits<NetConstruction::POLYNOMIAL>::SizeParameter sizeParameter)
    {
        std::ostringstream stream;
        stream << "Modulus: " << sizeParameter << std::endl;
        return stream.str();
    }
}


/**
 * Virtual base class for search task.
 * @tparam NC Net construction type (eg. SOBOL, POLYNOMIAL, EXPLICIT, ...).
 * @tparam ET Embedding type (UNILEVEL or MULTILEVEL)
 * @tparam OBSERVER Template template parameter whose template parameter is a net construction type
 *                  (eg. SOBOL, POLYNOMIAL, EXPLICIT, ...). Should mimick the MinimumObserver class template.
 */
template <NetConstruction NC, EmbeddingType ET, template <NetConstruction> class OBSERVER = MinimumObserver> 
class Search : public Task {

public:

    /// Observer of the search
    typedef OBSERVER<NC> Observer;

    /** Virtual default destructor.
     */ 
    virtual ~Search() = default;

    /**
     * Executes the search.
     */ 
    virtual void execute() override = 0;

    /**
     * Resets the search.
     */ 
    virtual void reset() override
    {
        m_observer->reset();
        m_bestNet = DigitalNetConstruction<NC>(0,m_sizeParameter);
        m_bestMerit = std::numeric_limits<Real>::infinity();
    }

    /// Signal emitted when a net has been selected.

    typedef boost::signals2::signal<void (const Search<NC, ET, OBSERVER>&)> OnNetSelected;

    /// Signal emitted when the search has failed.
    typedef boost::signals2::signal<void (const Search<NC, ET, OBSERVER>&)> OnFailedSearch;

    /**
     * Constructor.
     * @param dimension Dimension of the searched net.
     * @param sizeParameter Size parameter of the searched net.
     * @param verbose Verbosity level.
     * @param earlyAbortion Early-abortion switch. If true, the computations will be stopped if the net is worse than the best one so far.
     */
    Search( Dimension dimension, 
            typename NetConstructionTraits<NC>::SizeParameter sizeParameter,
            int verbose = 0,
            bool earlyAbortion = false ):
        m_onNetSelected(new OnNetSelected),
        m_onFailedSearch(new OnFailedSearch),
        m_dimension(dimension),
        m_sizeParameter(sizeParameter),
        m_nRows(NetConstructionTraits<NC>::nRows(m_sizeParameter)),
        m_nCols(NetConstructionTraits<NC>::nCols(m_sizeParameter)),
        m_bestNet(0, m_sizeParameter),
        m_bestMerit(std::numeric_limits<Real>::infinity()),
        m_observer(new Observer(sizeParameter, verbose-2)),
        m_verbose(verbose),
        m_earlyAbortion(earlyAbortion)
        {};

    /**
     * Constructor.
     * @param dimension Dimension of the searched net.
     * @param baseNet Net from which to start the search.
     * @param verbose Verbosity level.
     * @param earlyAbortion Early-abortion switch. If true, the computations will be stopped if the net is worse than the best one so far.
     */
    Search( Dimension dimension, 
            std::unique_ptr<DigitalNetConstruction<NC>> baseNet,
            int verbose = 0,
            bool earlyAbortion = false ):
        m_onNetSelected(new OnNetSelected),
        m_onFailedSearch(new OnFailedSearch),
        m_dimension(dimension),
        m_sizeParameter(baseNet->sizeParameter()),
        m_nRows(NetConstructionTraits<NC>::nRows(m_sizeParameter)),
        m_nCols(NetConstructionTraits<NC>::nCols(m_sizeParameter)),
        m_bestNet(0, m_sizeParameter),
        m_bestMerit(std::numeric_limits<Real>::infinity()),
        m_observer(new Observer(std::move(baseNet), verbose-2)),
        m_verbose(verbose),
        m_earlyAbortion(earlyAbortion)
        {};

    /** Default move constructor. 
     * Deletes the implicit copy constructor.
     */ 
    Search(Search&& ) = default;

    /** 
     * Returns the dimension of the searched net.
    */
    Dimension dimension() const
    { return m_dimension; }

    /** 
     * Returns the number of rows of the searched generating matrices. 
     */
    unsigned int nRows() const {return m_nRows; }

    /** 
     * Returns the number of columns of the searched generating matrices.
     */ 
    unsigned int nCols() const {return m_nCols; }

    /** 
     * Returns the best net found by the search. 
     */
    const DigitalNetConstruction<NC>& bestNet() const
    { return m_bestNet; }

    /** 
     * Returns the best merit found by the search. ``
     */
    Real bestMeritValue() const
    { return m_bestMerit; }

    bool hasFoundNet() const
    {
        return m_observer->hasFoundNet();
    }

    /**
     *  Returns the best net found by the search task.
     */
    virtual std::string outputNet(OutputFormat outputFormat, OutputMachineFormat outputMachineFormat, unsigned int interlacingFactor) const override
    { return bestNet().format(outputFormat, outputMachineFormat, interlacingFactor); }

    /**
     *  Returns information about the task
     */
    virtual std::string format() const override
    {
        std::string res;
        std::ostringstream stream;
        stream << "Task: NetBuilder Search - Net Construction : " << NetConstructionTraits<NC>::name << std::endl;
        stream << "Number of components: " << dimension() << std::endl;
        stream << output<NC>(m_sizeParameter);
        stream << "Number of rows: " << nRows() << " - Number of columns: " << nCols() << std::endl;
        res += stream.str();
        stream.str(std::string());
        return res;
    };

    /**
     * Returns the best merit value found by the search task.
     */
    virtual Real outputMeritValue() const override
    { return bestMeritValue(); }

    /** 
     * Returns a reference to the minimum-element observer. 
     */
    Observer& observer()
    { return *m_observer; }

    /** 
     * Returns a const qualified reference the minimum-element observer. 
     */
    const Observer& observer() const
    { return *m_observer; }

    /**
     * Net-selected signal.
     * Emitted when a net has been selected by the search algorithm.
     */
    OnNetSelected& onNetSelected()
    { return *m_onNetSelected; }

    /**
     * Net-selected signal.
     * Emitted when a net has been selected by the search algorithm.
     */
    const OnNetSelected& onNetSelected() const
    { return *m_onNetSelected; }

    /**
     * Failed search signal.
     * Emitted when no net has been selected by the search algorithm.
     */
    OnFailedSearch& onFailedSearch()
    { return *m_onFailedSearch; }

    /**
     * Failed search signal.
     * Emitted when no net has been selected by the search algorithm.
     */
    const OnFailedSearch& onFailedSearch() const
    { return *m_onFailedSearch; }

    /** 
     * Returns a const qualified reference to the figure of merit. 
     */
    virtual const FigureOfMerit::FigureOfMerit& figureOfMerit() const = 0;

    /** 
     * Returns a const qualified reference to the size parameter of the search.
     */ 
    const typename NetConstructionTraits<NC>::SizeParameter& sizeParameter() const {return m_sizeParameter;}

    protected:

        /**
         * Selects a new best net and emits an OnNetSelected signal.
         */
        void selectBestNet(const DigitalNetConstruction<NC>& net, Real merit)
        {
            m_bestNet = net;
            m_bestMerit = merit;
            onNetSelected()(*this);
        }

        std::unique_ptr<OnNetSelected> m_onNetSelected; // onNetSelected signal
        std::unique_ptr<OnFailedSearch> m_onFailedSearch; // onFailedSearch signal
        Dimension m_dimension; // dimension of the search
        typename NetConstructionTraits<NC>::SizeParameter m_sizeParameter; // size parameter of the search
        unsigned int m_nRows; // number of rows of the generating matrices
        unsigned int m_nCols; // number of columns of the generating matrices
        DigitalNetConstruction<NC> m_bestNet; // best net 
        Real m_bestMerit; // best merit
        std::unique_ptr<Observer> m_observer; // minimum observer
        int m_verbose; // verbosity level
        bool m_earlyAbortion; // early abortion switch
        
};

}}

#endif
