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

#ifndef NETBUILDER__FIGURE_OF_MERIT_BIT__PROJECTION_DEPENDENT_EVALUATOR
#define NETBUILDER__FIGURE_OF_MERIT_BIT__PROJECTION_DEPENDENT_EVALUATOR

#include "netbuilder/FigureOfMerit/WeightedFigureOfMerit.h"

namespace NetBuilder { namespace FigureOfMerit {

/** 
 * Class to implement the evaluation of specific projection-dependent weighted figure of merit where
 * the merits of the subprojections of order one less are used to compute the merit of a bigger projection, for instance
 * the t-value of subprojections. 
 * @tparam PROJDEP Template parameter representing the projection-dependent merit.
 */ 
template <typename PROJDEP>
class ProjectionDependentEvaluator : public FigureOfMeritCBCEvaluator 
{


    public:

        /** 
         * Constructor.
         * @param figure Pointer to the figure of merit.
         */ 
        ProjectionDependentEvaluator(WeightedFigureOfMerit<PROJDEP>* figure):
                    m_figure(figure),
                    m_numCoordinates(0),
                    m_maxNumCoordinates(0),
                    m_maxCardinal(m_figure->projDepMerit().maxCardinal())
        {};

        /** 
         * Destructor. */
        ~ProjectionDependentEvaluator()
        {
            for(Dimension dim = 0; dim < m_maxNumCoordinates; ++dim)
            {
                ProjectionNode* it = m_roots[dim];
                ProjectionNode* nextIt = nullptr;
                do
                {
                    nextIt = it->getNextNode();
                    delete it;
                    it = nextIt;
                }
                while(it != nullptr);
            }
        }

        /** 
         * Computes the figure of merit for the given \c net for the given \c dimension (partial computation), 
         * starting from the initial value \c initialValue.
         *  @param net Net to evaluate.
         *  @param dimension Dimension to compute.
         *  @param initialValue Initial value of the merit.
         *  @param verbose Verbosity level.
         */ 
        virtual MeritValue operator() (const DigitalNet& net, Dimension dimension, MeritValue initialValue, int verbose = 0) override
        {
            unsigned int nLevels = PROJDEP::numLevels(net); // determine the number of levels

            auto acc = m_figure->accumulator(std::move(initialValue));

            ProjectionNode* it = m_roots[dimension]; // iterator over the nodes
            do
            {   
                Real weight = it->getWeight();
                
                if (PROJDEP::size(it->getSubProjCombination()) < nLevels) // resize the subprojections combination if required
                {
                    PROJDEP::resize(it->getSubProjCombination(), nLevels);
                }

                it->updateSubProjCombination(); // update the subprojection combination

                LatticeTester::Coordinates proj = it->getProjectionRepresentation();

                auto grossMerit = m_figure->projDepMerit()(net, proj ,it->getSubProjCombination()); // compute the merit of the projection

                Real merit = m_figure->projDepMerit().combine(grossMerit, net, proj); // combine in a single merit value

                acc.accumulate(weight,merit,1);

                if (!onProgress()(acc.value()))  // if someone is listening, may tell that the computation is useless
                {
                    acc.accumulate(std::numeric_limits<Real>::infinity(), merit, 1); // set the merit to infinity
                    onAbort()(net); // abort the computation
                    break;
                }

                it->setMeritTmp(grossMerit); // update the merit of the node
                it = it->getNextNode(); // skip to next node
            }
            while(it != nullptr);

            return acc.value();
        }

        /**     
         * Resets the evaluator and prepare it to evaluate a new net.
         */ 
        virtual void reset() override { m_numCoordinates=0; }

        /**
         * Tells the evaluator that the last net was the best so far and store the relevant information
         */
        virtual void lastNetWasBest() override
        {
            saveMerits(m_numCoordinates-1);
        }

        /**
         * Tells the evaluator that no more net will be evaluate for the current dimension,
         * store information about the best net for the dimension which is over and prepare data structures
         * for the next dimension.
         */ 
        virtual void prepareForNextDimension() override
        {
            if (m_numCoordinates<m_maxNumCoordinates)
            {
                ++m_numCoordinates;
            }
            else if (m_numCoordinates==m_maxNumCoordinates)
            {
                extend();
                ++m_numCoordinates;
            }
            else if (m_numCoordinates > m_maxNumCoordinates)
            {
                throw std::runtime_error("In projection-dependent figure of merit evaluator: evaluator is ill-formed.");
            }
        }

    private:

        /** 
         * Represents a projection. The node is linked to the projections with order one less and with the following
         * projection in the evaluation order. The node store some merit values.
         */ 
        struct ProjectionNode
        {
            /// Type of merit value storage.
            typedef typename PROJDEP::Merit MeritStorage;

            /// Type of the combination of the merits of the subprojections.
            typedef typename PROJDEP::SubProjCombination SubProjCombination; 

            /** 
             * Constructor.
             * @param dimension Highest coordinate in all the projections of the layer.
             * @param cardinal Cardinal of the projection.
             * @param weight Importance of the projection in the figure of merit.
             */ 
            ProjectionNode(Dimension dimension, unsigned int cardinal, double weight):
                m_weight(weight),
                m_dimension(dimension),
                m_cardinal(cardinal)
            {};

            /** 
             * Returns the cardinal of the projection represented by the node.
             */ 
            unsigned int getCardinal() const { return m_cardinal; }

            /** 
             * Returns the maximum dimension, that is the highest coordinate in all the projections of the layer.
             */ 
            Dimension getMaxDimension() const { return m_dimension; }

            /** Returns a pointer to the next projection to evaluate. May be null.
             */ 
            ProjectionNode* getNextNode() const { return m_nextNode; }

            /** Set the next node of the current node.
             * @param node is a pointer to the next node.
             */ 
            void setNextNode(ProjectionNode* node){ m_nextNode = node; }

            /** 
             * Returns a vector of pointers to the subprojections whose cardinal is one less than the
             * projection's.
             */ 
            const std::vector<ProjectionNode*>& getMotherNodes() const { return m_mothersNodes; }

            /** 
             * Returns the projection represented by the node.
             */ 
            LatticeTester::Coordinates getProjectionRepresentation() const { 
                LatticeTester::Coordinates res;
                accumulateProjectionRepresentation(res);
                return res;
            }

            /**
             * Add a mother to the projection node.
             * @param mother Pointer to the new mother.
             */ 
            void addMother(ProjectionNode* mother) { m_mothersNodes.push_back(mother); }

            /** 
             * Returns the weight of the projection represented by the node.
             */ 
            Real getWeight() const { return m_weight; }


            /** Returns the stored merit of the projection represented by the node.
             */ 
            MeritStorage getMeritMem() const {return m_meritMem; }

            /** Returns the temporary merit of the projection represented by the node.
             */ 
            MeritStorage getMeritTmp() const {return m_meritTmp; }


            /** 
             * Returns the maximum of the merits of the subprojections.
             */ 
            SubProjCombination& getSubProjCombination() { return m_subProjCombination; }

            /** 
             * Set the temporary merit of the node to be the given merit
             * @param merit is the merit to assign to the node.
             */ 
            void setMeritTmp(MeritStorage merit) {m_meritTmp = merit; }

            /** 
             * Save the temporary merit in the m_meritMem field. 
             */ 
            void saveMerit() {m_meritMem = m_meritTmp; }

            /** 
             * Updates the combination of the merits of the subprojections (mothers). Note that for
             * subprojections which also contain getMaxDimension(), the temporary merit is used
             * whereas for other nodes, the stored merit is used. This allows component-by-component
             * evaluation for several nets with a unique datastructure..
             */ 
            void updateSubProjCombination()
            {
                if (getCardinal() > 1)
                {
                    PROJDEP::setToZero(m_subProjCombination);
                    for (auto const* m : m_mothersNodes)
                    {
                        if (m->getMaxDimension() < m_dimension)
                        {
                            PROJDEP::update(m->getMeritMem(), m_subProjCombination);
                        }
                        else{
                            PROJDEP::update(m->getMeritTmp(), m_subProjCombination);
                        } 
                    }
                }
            }

            /** 
             * Overloads operator < to compare projections. A projection is smaller (less important) than another one if they have the same cardinal and its weight is smaller
             * of if it has a strictly bigger cardinal.
             */ 
            bool operator>(const ProjectionNode &b) const
            {
                return (getCardinal() == b.getCardinal()) ? 
                            (getWeight() > b.getWeight()) : 
                            (getCardinal() < b.getCardinal());
            }

            /** 
             * Comparison between pointers to nodes by dereferencing. Used to sort by decreasing importance.
             */ 
            static bool compareNodePointers(const ProjectionNode* a, const ProjectionNode* b){
                return (*a > * b);
            }
                    
            /** 
             * Overloading of the << operator to print projection nodes.
             */
            friend std::ostream& operator<<(std::ostream& os, const ProjectionNode& dt)
            {
                os << "projection: " << dt.getProjectionRepresentation() << " - weight: " << dt.getWeight();
                return os;
            }

            double m_weight; // weight of the projection
            Dimension m_dimension; // last dimension (highest coordinate in the projection)
            unsigned int m_cardinal; // cardinal of the projection

            ProjectionNode* m_nextNode = nullptr; // pointer to the next projection to evaluate
            std::vector<ProjectionNode*> m_mothersNodes; // pointers to the subprojections whose cardinal is one less

            SubProjCombination m_subProjCombination; // combination of the merits of the subprojections

            MeritStorage m_meritMem; // stored merit
            MeritStorage m_meritTmp; // temporay merit

            void accumulateProjectionRepresentation(LatticeTester::Coordinates& projection) const
            {
                projection.insert(m_dimension);
                if(m_cardinal>1)
                {
                    m_mothersNodes[0]->accumulateProjectionRepresentation(projection);
                }
            }
        };

        /** 
         * Extends by one dimension the evaluator. This creates new nodes corresponding to the new projections to consider
         * while evaluating figures of merits.
         */ 
        void extend(){
            ++m_maxNumCoordinates; // increase maximal number of coordinates
            std::vector<ProjectionNode*> newNodes; // to store new nodes

            std::map<LatticeTester::Coordinates,ProjectionNode*> mapsToNodes; // map between new projections and new nodes

            // create projection {m_numCoordinates} 
            LatticeTester::Coordinates proj1DRep;
            proj1DRep.insert(m_maxNumCoordinates-1);
            double weight = m_figure->weights().getWeight(proj1DRep);
            ProjectionNode* proj1D = new ProjectionNode(m_maxNumCoordinates-1, (unsigned int) proj1DRep.size(), weight);

            if(m_maxNumCoordinates==1){
                m_roots.push_back(proj1D);
                return;
            }

            newNodes.push_back(proj1D);

            mapsToNodes.insert(std::pair<LatticeTester::Coordinates,ProjectionNode*>(std::move(proj1DRep),proj1D));


            for(unsigned int i = 0; i < m_maxNumCoordinates-1; ++i) // for each previous dimensions
            {
                ProjectionNode* it = m_roots[i];
                do
                {   if (it->getCardinal() <= m_maxCardinal-1)
                    {
                        LatticeTester::Coordinates projectionRep = it->getProjectionRepresentation(); // consider the projection
                        projectionRep.insert(m_maxNumCoordinates-1);
                        double weight =  m_figure->weights().getWeight(projectionRep);
                        ProjectionNode* newNode = new ProjectionNode(m_maxNumCoordinates-1, (unsigned int) projectionRep.size() , weight); // create the node
                        newNode->addMother(it);
                        mapsToNodes.insert(std::pair<LatticeTester::Coordinates,ProjectionNode*>(std::move(projectionRep),newNode));
                        newNodes.push_back(newNode);
                    }
                    it = it->getNextNode();
                }
                while(it != nullptr);
            }

            std::sort(newNodes.begin(), newNodes.end(), ProjectionNode::compareNodePointers); // sort the nodes by increasing cardinal and decreasing weights

            m_roots.push_back(newNodes[0]); // add the root corresponding to the dimension

            for(size_t i = 0; i < newNodes.size()-1; ++i)
            {
                newNodes[i]->setNextNode(newNodes[i+1]); // link the new nodes
            }

            for (ProjectionNode* node : newNodes) // for each new nodes
            {   
                LatticeTester::Coordinates tmp = node->getProjectionRepresentation();
                for(unsigned int i = 0; i < m_maxNumCoordinates-1; ++i) // link to mothers which contains m_numCoordinates
                {
                    if (tmp.find(i) != tmp.end())
                    {
                        tmp.erase(i);
                        auto foo = mapsToNodes.find(tmp)->second;
                        node->addMother(foo);
                        tmp.insert(i);
                    }
                }
            }
        }

        /** Save the merits of all the nodes corresponding to the \c dimension.
         * @param dimension Dimension of the nodes.
         */  
        void saveMerits(Dimension dimension)
        {
            ProjectionNode* it = m_roots[dimension];
            do
            {
                it->saveMerit();
                it = it->getNextNode();
            }
            while(it != nullptr);
        }

        /** 
         * Save the merits of all the nodes in the evaluator.
         */ 
        void saveMerits()
        {
            for(unsigned int i = 0; i < m_numCoordinates; ++i)
            {
                saveMerits(i);
            }
        }

        WeightedFigureOfMerit<PROJDEP> * m_figure;

        Dimension m_numCoordinates; 
        Dimension m_maxNumCoordinates;
        unsigned int m_maxCardinal; 
        std::vector<ProjectionNode*> m_roots; // pointer to the first node of each layer (one by dimension)
};

}}

#endif