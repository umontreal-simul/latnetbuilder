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

#ifndef NETBUILDER__TASK__SEARCH_H
#define NETBUILDER__TASK__SEARCH_H

#include "netbuilder/Task/BaseTask.h"
#include "netbuilder/Task/MinObserver.h"

#include "netbuilder/FigureOfMerit/FigureOfMerit.h"

#include <ostream>
#include <memory>

namespace NetBuilder { namespace Task {


template <NetConstruction NC> 
class Search : public BaseTask {

public:

    virtual ~Search() = default;

    virtual void execute() override = 0;

    virtual void reset() override
    {
        m_minObserver.reset();
        m_bestNet = DigitalNetConstruction<NC>(0,m_nRows,m_nCols);
        m_bestMerit = std::numeric_limits<Real>::infinity();
    }

    typedef boost::signals2::signal<void (const Search<NC>&)> OnNetSelected;
    typedef boost::signals2::signal<void (const Search<NC>&)> OnFailedSearch;

    Search( Dimension dimension, 
            unsigned int nRows, 
            unsigned int nCols, 
            std::unique_ptr<FigureOfMerit::FigureOfMerit> figure,
            unsigned int verbose = 0 ):
        m_onNetSelected(new OnNetSelected),
        m_onFailedSearch(new OnFailedSearch),
        m_dimension(dimension),
        m_nRows(nRows),
        m_nCols(nCols),
        m_figure(std::move(figure)),
        m_bestNet(nRows,nCols),
        m_bestMerit(std::numeric_limits<Real>::infinity()),
        m_minObserver(new MinObserver<NC>(nRows,nCols)),
        m_verbose(verbose)
        {};

    Search( Dimension dimension, 
            unsigned int m,
            std::unique_ptr<FigureOfMerit::FigureOfMerit> figure,
            unsigned int verbose = 0):
        Search(dimension,m,m,std::move(figure))
    {};

    Search(Search&& ) = default;

    /** Returns the dimension of the searched net.
    */
    Dimension dimension() const
    { return m_dimension; }

    /** Returns the number of rows of the searched generating matrices. */
    unsigned int nRows() const {return m_nRows; }

    /** Returns the number of columns of the searched generating matrices. */
    unsigned int nCols() const {return m_nCols; }

    /** Returns the best net found by the search. */
    const DigitalNetConstruction<NC>& bestNet() const
    { return m_bestNet; }

    /** Returns the best merit found by the search. */
    Real bestMeritValue() const
    { return m_bestMerit; }

    /**
    * Returns the best net found by the search task.
    */
    virtual const DigitalNet& netOutput() const override
    { return bestNet(); }

    /**
    * Returns the best merit value found by the search task.
    */
    virtual Real meritValueOutput() const override
    { return bestMeritValue(); }

    /** Returns a reference to the minimum-element observer. */
    MinObserver<NC>& minObserver()
    { return *m_minObserver; }

    /** Returns a const qualified reference the minimum-element observer. */
    const MinObserver<NC>& minObserver() const
    { return *m_minObserver; }

    /**
    * Net-selected signal.
    *  Emitted when a net has been selected by the search algorithm.
    */
    OnNetSelected& onNetSelected()
    { return *m_onNetSelected; }

    const OnNetSelected& onNetSelected() const
    { return *m_onNetSelected; }

    /**
    * Failed search signal.
    *  Emitted when no net has been selected by the search algorithm.
    */
    OnFailedSearch& onFailedSearch()
    { return *m_onFailedSearch; }

    const OnFailedSearch& onFailedSearch() const
    { return *m_onFailedSearch; }

    /** Returns a const qualified reference to the figure of merit. */
    const FigureOfMerit::FigureOfMerit& figureOfMerit() const 
    {
            return *m_figure;
    }

    protected:

        /**
        * Selects a new best net and emits an OnNetSelected signal.
        */
        void selectBestNet(const DigitalNetConstruction<NC> net, Real merit)
        {
            m_bestNet = net;
            m_bestMerit = merit;
            onNetSelected()(*this);
        }

        std::unique_ptr<OnNetSelected> m_onNetSelected;
        std::unique_ptr<OnFailedSearch> m_onFailedSearch;
        Dimension m_dimension;
        unsigned int m_nRows;
        unsigned int m_nCols;
        std::unique_ptr<FigureOfMerit::FigureOfMerit> m_figure;
        DigitalNetConstruction<NC> m_bestNet;
        Real m_bestMerit;
        std::unique_ptr<MinObserver<NC>> m_minObserver;
        unsigned int m_verbose;
};

}}

#endif
