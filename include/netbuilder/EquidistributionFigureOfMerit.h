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

#ifndef NET_BUILDER__WEIGHTED_EQUIDISTRIBUTION_FIGURE_OF_MERIT_H
#define NET_BUILDER__WEIGHTED_EQUIDISTRIBUTION_FIGURE_OF_MERIT_H

#include "netbuilder/Types.h"
#include "netbuilder/DigitalNet.h"

#include "latcommon/Weights.h"
#include "latcommon/Coordinates.h"

#include "latbuilder/Functor/AllOf.h"
#include "latbuilder/Accumulator.h"

#include <boost/signals2.hpp>

namespace NetBuilder{

using LatBuilder::Functor::AllOf;
using LatBuilder::Accumulator;


template<typename FIGURE>
class EquidistributionFigureOfMeritEvaluator ;

template <typename PROJDEP, template <typename> class ACC>
class EquidistributionFigureOfMerit{

    public:

        typedef Real MeritValue;

        EquidistributionFigureOfMerit(std::unique_ptr<LatCommon::Weights> weights,  unsigned int maxCardinal = 2, PROJDEP projdep = PROJDEP()):
            m_weights(std::move(weights)),
            m_maxCardinal(maxCardinal),
            m_projDepMerit(projdep)
        {};

        const LatCommon::Weights& weights() const { return *m_weights; }

        /**
        * Returns the projection-dependent figure of merit \f$D_{\mathfrak u}^2\f$.
        */
        const PROJDEP& projDepMerit() const { return m_projDepMerit; }

        unsigned int maxCardinal() const { return m_maxCardinal; }
       /**
        * Creates a new accumulator.
        *
        * \param initialValue Initial accumulator value.
        */
        template <typename T>
        static Accumulator<ACC, T> accumulator(T initialValue)
        { return Accumulator<ACC, T>(std::move(initialValue)); }


   EquidistributionFigureOfMeritEvaluator<EquidistributionFigureOfMerit>
   evaluator() const
   {   return EquidistributionFigureOfMeritEvaluator<EquidistributionFigureOfMerit>(*this); }

    private:
        std::unique_ptr<LatCommon::Weights> m_weights;
        PROJDEP m_projDepMerit;
        unsigned int m_maxCardinal;
};

template<typename FIGURE>
class EquidistributionFigureOfMeritEvaluator {

    public:

        typedef LatCommon::Coordinates Coordinates;

        typedef typename FIGURE::MeritValue MeritValue;

        typedef boost::signals2::signal<bool (const MeritValue&), LatBuilder::Functor::AllOf> OnProgress;
        typedef boost::signals2::signal<void (const DigitalNet)> OnAbort;

        /**
        * Constructor.
        */
        EquidistributionFigureOfMeritEvaluator(const FIGURE& figure):
            m_onProgress(new OnProgress),
            m_onAbort(new OnAbort),
            m_figure(figure),
            m_dimension(0),
            m_maxCardinal(m_figure.maxCardinal())
        {};

        EquidistributionFigureOfMeritEvaluator(EquidistributionFigureOfMeritEvaluator&&) = default;

        /// \name Signals
        //@{
        /**
        * Progress signal.
        *
        * Emitted after each projection-dependent contribution to the weighted
        * figure of merit is processed.
        * The signal argument is the cumulative value of the weighted figure of
        * merit.  If any of the signal slots returns \c false, the computation of
        * the figure of merit will be aborted.
        */
        OnProgress& onProgress() const { return *m_onProgress; }

        /**
        * Abort signal.
        *
        * Emitted if the computation of the weighted figure of merit is aborted.
        * The signal argument is the lattice definition for which the computation
        * was aborted.
        */
        OnAbort& onAbort() const { return *m_onAbort; }
        //@}


        /** Extend by one dimension the ComputationScheme. This creates new nodes corresponding to the new projections to consider
         * while evaluating figures of merits.
         */ 
        void extend(){
            ++m_dimension; // increase maximal dimension
            std::vector<Node*> newNodes; // to store new nodes

            std::map<Coordinates,Node*> mapsToNodes; // map between new projections and new nodes


            m_validFlags.push_back(false);

            // create projection {m_dimension} 
            Coordinates proj1DRep;
            proj1DRep.insert(m_dimension-1);
            double weight = m_figure.weights().getWeight(proj1DRep);
            Node* proj1D = new Node(proj1DRep,m_dimension,weight);

            if(m_dimension==1){
                m_roots.push_back(proj1D);
                return;
            }

            newNodes.push_back(proj1D);

            mapsToNodes.insert(std::pair<Coordinates,Node*>(std::move(proj1DRep),proj1D));


            for(int i = 0; i < m_dimension-1; ++i) // for each previous dimensions
            {
                Node* it = m_roots[i];
                do
                {   if (it->getProjectionRepresentation().size() <= m_maxCardinal-1)
                    {
                        Coordinates projectionRep = it->getProjectionRepresentation(); // consider the projection
                        projectionRep.insert(m_dimension-1);
                        double weight =  m_figure.weights().getWeight(projectionRep);
                        Node* newNode = new Node(projectionRep,m_dimension,weight); // create the node
                        newNode->addMother(it);
                        mapsToNodes.insert(std::pair<Coordinates,Node*>(std::move(projectionRep),newNode));
                        newNodes.push_back(newNode);
                    }
                    it = it->getNextNode();
                }
                while(it != nullptr);
            }

            std::sort(newNodes.begin(),newNodes.end(),compareNodePointers); // sort the nodes by increasing cardinal and decreasing weights

            m_roots.push_back(newNodes[0]); // add the root corresponding to the dimension

            for(size_t i = 0; i < newNodes.size()-1; ++i)
            {
                newNodes[i]->setNextNode(newNodes[i+1]); // link the new nodes
            }
            for (auto& kv : mapsToNodes) // for each new nodes
            {   
                Coordinates tmp = kv.first;
                for(int i = 0; i < m_dimension-1; ++i) // link to mothers which contains m_dimension
                {
                    if (tmp.find(i) != tmp.end())
                    {
                        tmp.erase(i);
                        kv.second->addMother(mapsToNodes.find(tmp)->second);
                        tmp.insert(i);
                    }
                }
            }
        }

        /** Extend the ComputationSchemme up to the given last dimension. 
         * @param dimension is the highest dimension in the projections
         * @see extend
         */ 
        void extendUpToDimension(unsigned int dimension)
        {
            for(unsigned int i = m_dimension; i < dimension; ++i)
            {
                extend();
            }
        }

        ~EquidistributionFigureOfMeritEvaluator()
        {
            for(unsigned int dim = 0; dim < m_dimension; ++dim)
            {
                Node* it = m_roots[dim];
                Node* nextIt = nullptr;
                do
                {
                    nextIt = it->getNextNode();
                    delete it;
                    it = nextIt;
                }
                while(it != nullptr);
            }
        }

        /** Returns a map containing all the merits stored in the nodes corresponding to the given dimension, 
         * that is those whose highest element is equal to dimension.
         * @param dimension is the dimension to consider
         * @return a map between projections and merits
         */ 
        std::map<Coordinates,int> getAllMerits(unsigned int dimension) const
        {
            std::map<Coordinates,int> meritsMap;

            assert(m_validFlags[dimension-1]);

            const Node* it = m_roots[dimension-1];
            do
            {
                meritsMap.insert(std::pair<Coordinates,int>(it->getProjectionRepresentation(),it->getMeritMem()));
                it = it->getNextNode();
            }
            while(it != nullptr);
            return meritsMap;
        }

        /** Save the merits of all the nodes corresponding to the given dimension.
         * @param dimension is the dimension to consider
         */  
        void saveMerits(unsigned int dimension)
        {
            Node* it = m_roots[dimension-1];
            do
            {
                it->saveMerit();
                it = it->getNextNode();
            }
            while(it != nullptr);
        }

        /** Save the merits of all the nodes in the scheme.
         */ 
        void saveMerits()
        {
            for(int i = 0; i< m_dimension; ++i)
            {
                Node* it = m_roots[i];
                do
                {
                    it->saveMerit();
                    it = it->getNextNode();
                }                 
                while(it != nullptr);
            }
        }

        /** Returns true if the figure of merit is fully computed for dimensions lower than maxDimension. Returns false 
         * otherwise.
         * @param maxDimension is the maximal dimension to consider
         */ 
        bool isValid(unsigned int maxDimension) const
        {
            if(maxDimension > m_dimension)
            {
                return false;
            }

            for(unsigned int i = 0; i < maxDimension; ++i)
            {
                if(!m_validFlags[i])
                {
                    return false;
                }
            }

            return true;
        }

        /** Set the scheme to be valid for the given dimension. This can only be done if the lower dimensions are valid to.
         * @param dimension is the dimension to set valid
         */ 
        void validate(unsigned int dimension)
        {
            assert(dimension <= m_dimension);
            assert(isValid(dimension-1));
            m_validFlags[dimension-1] = true;
        }

        /** Set the scheme to be unvalid for the given dimension and all the upper dimensions.
         * @param dimension is the dimension to consider
         */
        void invalidate(unsigned int dimension)
        {
            assert(dimension >= 1);
            for(unsigned int i = dimension-1; i < m_dimension; ++i)
            {
                if (!m_validFlags[i])
                {
                    return ;
                }
                m_validFlags[i] = false;
            }
        } 


        MeritValue operator() (const DigitalNet& net, unsigned int dimension, MeritValue initialValue, bool verbose = false)
        {
            extendUpToDimension(dimension);

            assert(isValid(dimension-1)); // check that the previous dimensions were correctly computed
            assert(net.dimension()>=dimension);
            invalidate(dimension + 1); // declare invalid the merits stored for upper dimensions

            auto acc = m_figure.accumulator(std::move(initialValue));

            std::vector<GeneratingMatrix> allGeneratingMatrices;

            for (int dim = 1; dim <= dimension ; ++dim) // for each dimension
            {
                allGeneratingMatrices.push_back(net.generatingMatrix(dim));
            }

            Node* it = m_roots[dimension-1]; // iterator over the nodes
            do
            {   
                Real weight = it->getWeight();
    
                std::vector<GeneratingMatrix> genMatrices;
                for(const auto& coord : it->getProjectionRepresentation())
                {
                    genMatrices.push_back(allGeneratingMatrices[coord]);
                }

                it->updateMaxMeritsSubProj();
                Real merit = m_figure.projDepMerit()(std::move(genMatrices),it->getMaxMeritsSubProj(), false); // compute the merit of the projection
                
                if(verbose)
                {
                    std::cout << *it << " - merit sub: " << it->getMaxMeritsSubProj() << " - merit: " << merit << std::endl;
                }

                acc.accumulate(weight,merit,1);

                if (!onProgress()(acc.value())) 
                {
                    acc.accumulate(std::numeric_limits<Real>::infinity(), merit, 1);
                    onAbort()(net);
                    validate(dimension);
                    return acc.value();
                }

                it->setMeritTmp(merit); // update the merit of the node
                it = it->getNextNode(); // skip to next node
            }
            while(it != nullptr);

            validate(dimension);
            saveMerits();
            return acc.value();

            
    }

    private:
        std::unique_ptr<OnProgress> m_onProgress;
        std::unique_ptr<OnAbort> m_onAbort;
        const FIGURE& m_figure;

         /** Private class to represent each element of \fJ_d\f. 
         */ 
        class Node
        {

            public:
                /** Constructor for class Node.
                 * @param projRep is the bitset representation of the projection
                 * @param dimension is the equivalent of \fd\f
                 * @param weight is the importance of the projection in the figure of merit
                 */ 
                Node(const Coordinates& projRep, int dimension, double weight):
                    m_weight(weight),
                    m_projRep(projRep),
                    m_dimension(dimension)
                {
                    m_cardinal = projRep.size(); // compute the cardinal
                }

                /** Returns the cardinal of the projection represented by the node.
                 */ 
                int getCardinal() const { return m_cardinal; }

                /** Returns the maximum dimension, that is the highest coordinate which is in the node.
                 */ 
                int getMaxDimension() const { return m_dimension; }

                /** Returns the weight of the projection represented by the node.
                 */ 
                double getWeight() const { return m_weight; }

                /** Returns the maximum of the merits of the subprojections.
                 */ 
                int getMaxMeritsSubProj() const { return m_maxMeritsSubProj; }

                /** Returns the stored merit of the projection represented by the node.
                 */ 
                int getMeritMem() const {return m_meritMem; }

                /** Returns the temporary merit of the projection represented by the node.
                 */ 
                int getMeritTmp() const {return m_meritTmp; }

                /** Returns a pointer to the next projection to evaluate. May be null.
                 */ 
                Node* getNextNode() const { return m_nextNode; }

                /** Returns a vector of pointers to the subprojections whose cardinal is one less than the
                 * projection's.
                 */ 
                std::vector<Node*> getMotherNodes() const { return m_mothersNodes; }

                /** Returns the bitset representation of the projection represented by the node.
                 */ 
                const Coordinates& getProjectionRepresentation() const { return m_projRep; }

                /** Set the temporary merit of the node to be the given merit
                 * @param merit is the merit to assign to the node.
                 */ 
                void setMeritTmp(int merit) {m_meritTmp = merit; }

                /** Set the next node of the current node.
                 * @param node is a pointer to the next node.
                 */ 
                void setNextNode(Node* node){ m_nextNode = node; }

                void addMother(Node* mother) { m_mothersNodes.push_back(mother); }

                /** Save the temporary merit in the m_meritMem field. 
                 */ 
                void saveMerit() {m_meritMem = m_meritTmp; }

                /** Update the maximum of the merits of the subprojections. Note that for
                 * subprojections which also contain getMaxDimension(), the temporary merit is used
                 * whereas for other nodes, the stored merit is used. This allows component-by-component
                 * evaluation for several nets using only one ComputationScheme.
                 */ 
                void updateMaxMeritsSubProj()
                {
                    m_maxMeritsSubProj=0;
                    for (auto const* m : m_mothersNodes)
                    {
                        if (m->getProjectionRepresentation().find(m_dimension) != m->getProjectionRepresentation().end())
                        {
                            m_maxMeritsSubProj = std::max(m->getMeritTmp(),m_maxMeritsSubProj);
                        }
                        else{
                            m_maxMeritsSubProj = std::max(m->getMeritMem(),m_maxMeritsSubProj);
                        } 
                    }
                }

                /** Overloading of the < operator to compare projections. A projection is smaller (less important) than another one if they have the same cardinal and its weight is smaller
                 * of if it has a strictly bigger cardinal.
                 */ 
                bool operator>(const Node &b) const
                {
                    return (getCardinal() == b.getCardinal()) ? 
                                (getWeight() > b.getWeight()) : 
                                (getCardinal() < b.getCardinal());
                }
                
                /** Overloading of the << operator to print projection nodes.
                 */
                friend std::ostream& operator<<(std::ostream& os, const Node& dt)
                {
                    os << "projection: " << dt.getProjectionRepresentation() << " - weight: " << dt.getWeight();
                    return os;
                }

            private:
                double m_weight; // weight of the projection
                Coordinates m_projRep; // bitset representation of the projection
                int m_dimension; // last dimension (highest coordinate in the projection)
                int m_cardinal; // cardinal of the projection

                Node* m_nextNode = nullptr; // pointer to the next projection to evaluate
                std::vector<Node*> m_mothersNodes; // pointers to the subprojections whose cardinal is one less
                int m_maxMeritsSubProj = 0; // maximum of the merits of the subprojections
                int m_meritMem = 0; // stored merit
                int m_meritTmp = 0; // temporay merit
        };

        /** Comparison between pointers to nodes by dereferencing. Used to sort by decreasing importance.
         */ 
        static bool compareNodePointers(const Node* a, const Node* b){
            return (*a > * b);
        }

        int m_dimension; // last dimension of J_d
        int m_maxCardinal; // maximal cardinality of projections to consider (alpha)
        std::vector<Node*> m_roots; // pointer to the first node to evaluate
        std::vector<bool> m_validFlags;

};

}

#endif 

