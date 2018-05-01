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

#ifndef NETBUILDER__TVALUE_PROJ_MERIT
#define NETBUILDER__TVALUE_PROJ_MERIT

#include "netbuilder/Types.h"
#include "netbuilder/Util.h"
#include "netbuilder/DigitalNet.h"
#include "netbuilder/TValueComputation.h"
#include "netbuilder/FigureOfMerit/WeightedFigureOfMerit.h"

#include "latcommon/Weights.h"
#include "latcommon/Coordinates.h"

#include <vector>
#include <memory>
#include <algorithm>
#include <string>

#include <functional>

namespace NetBuilder { namespace FigureOfMerit {

/** Template class representing a projection-dependent merit defined by the t-value of the projection
 */ 
template <NetEmbed NE>
class TValueProjMerit
{};

/** Template specialization in the case of simple nets.
 */ 
template <>
class TValueProjMerit<NetEmbed::SIMPLE>
{
    public:

        /** Construct a projection-dependent merit based on the t-value of projections.
         * @param maxCardinal is the maximum order of the subprojections to take into account
         */  
        TValueProjMerit(unsigned int maxCardinal):
            m_maxCardinal(maxCardinal)
        {};

        /** Returns the maximum order of the subprojections to take into account.
         */ 
        unsigned int maxCardinal() const { return m_maxCardinal; }

        /** Returns the name of the projection-dependent merit.
         */ 
        std::string name() const { return m_name ;}

        /**  Overload of << operator to print the name of the the projection-dependent merit on the given output stream
         */ 
        friend std::ostream& operator<<(std::ostream& os, const TValueProjMerit& dt)
        {
            os << "Proj dep merit: " << dt.name();
            return os;
        } 


        /** Computes the projection-dependent merit of the net for the given projection.
         * @param net is the digital net for which we want to compute the merit
         * @param projection is the projection to consider
         * @param maxMeritsSubProj is the maximal merit of the subprojections
         */ 
        Real operator()(const DigitalNet& net , const LatCommon::Coordinates& projection, unsigned int maxMeritsSubProj) const 
        {
            std::vector<GeneratingMatrix> mats;
            for(unsigned int dim : projection)
            {
                mats.push_back(net.generatingMatrix(dim+1));
            }
            return GaussMethod::computeTValue(std::move(mats),maxMeritsSubProj, false);
        }

    private:
        std::string m_name = "Projection-dependent merit: t-value (simple nets)"; // name of the projection-dependent merit
        unsigned int m_maxCardinal; // maximum order of subprojections to take into account 
};

/** Template specialization in the case of embedded nets.
 */ 
template <>
class TValueProjMerit<NetEmbed::EMBEDDED>
{
    public:

        /** Construct an projection-dependent merit based on the t-values of the projections.
         * @param maxCardinal is the maximum order of the subprojections to take into account
         */  
        TValueProjMerit(unsigned int maxCardinal, std::function<Real (const std::vector<unsigned int>&)> combiner):
            m_maxCardinal(maxCardinal),
            m_combiner(std::move(combiner))
        {};

        /** Returns the maximum order of the subprojections to take into account.
         */ 
        unsigned int maxCardinal() const { return m_maxCardinal; }

        /** Returns the name of the projection-dependent merit.
         */ 
        std::string name() const { return m_name;}

        /**  Overload of << operator to print the name of the the projection-dependent merit on the given output stream
         */ 
        friend std::ostream& operator<<(std::ostream& os, const TValueProjMerit& dt)
        {
            os << "Proj dep merit: " << dt.name();
            return os;
        } 

        /** Computes the projection-dependent merits (embedded) of the net for the given projection.
         * @param net is the digital net for which we want to compute the merit
         * @param projection is the projection to consider
         * @param maxMeritsSubProj is the maximal merit of the subprojections
         */ 
        std::vector<unsigned int> operator()(const DigitalNet& net, const LatCommon::Coordinates& projection, const std::vector<unsigned int>& maxMeritsSubProj) const 
        {
            return maxMeritsSubProj ; // TO IMPLEMENT
        }

        /** Combines the projection-dependent merits (embedded) into a single value merit.
         * @param merits are the embedded merits
         */ 
        Real combine(const std::vector<unsigned int>& merits) const { return m_combiner(merits) ; }

    private:
        std::string m_name = "Projection-dependent merit: t-value (embedded nets)";// name of the projection-dependent merit
        unsigned int m_maxCardinal; // maximum order of subprojections to take into account 

        std::function<Real (const std::vector<unsigned int>&)> m_combiner; 
        // function wrapper which combines embedded merits in a single value merit
};

template<>
class WeightedFigureOfMerit<TValueProjMerit<NetEmbed::SIMPLE>>::WeightedFigureOfMeritEvaluator : public FigureOfMeritEvaluator
{
    public:

        /** Constructor.
         * @param figure is a pointer to the figure of merit
         */ 
        WeightedFigureOfMeritEvaluator(WeightedFigureOfMerit* figure):
                    m_figure(figure),
                    m_dimension(0),
                    m_maxCardinal(m_figure->projDepMerit().maxCardinal())
        {};

        /** Destructor. */
        ~WeightedFigureOfMeritEvaluator()
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

        /** Computes the figure of merit for the given \c net for the given \c dimension (partial computation), 
          * starting from the initial value \c initialValue.
          * @param net is the net for which we compute the merit
          * @param dimension is the dimension for which we want to compute the merit
          * @param initialValue is the value from which to start
          * @param verbose controls the level of verbosity of the computation
          */ 
        virtual MeritValue operator() (const DigitalNet& net, unsigned int dimension, MeritValue initialValue, bool verbose = false)
        {

            extendUpToDimension(dimension);

            assert(isValid(dimension-1)); // check that the previous dimensions were correctly computed
            assert(net.dimension()>=dimension);
            invalidate(dimension + 1); // declare invalid the merits stored for upper dimensions

            auto acc = m_figure->accumulator(std::move(initialValue));

            Node* it = m_roots[dimension-1]; // iterator over the nodes
            do
            {   
                Real weight = it->getWeight();

                it->updateMaxMeritsSubProj();
                Real merit = m_figure->projDepMerit()(net,it->getProjectionRepresentation(),it->getMaxMeritsSubProj()); // compute the merit of the projection
                
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
            saveMerits(dimension);
            return acc.value();
    }

    private:
        WeightedFigureOfMerit* m_figure;

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
                Node(unsigned int dimension, unsigned int cardinal, double weight):
                    m_weight(weight),
                    m_cardinal(cardinal),
                    m_dimension(dimension)
                {};

                /** Returns the cardinal of the projection represented by the node.
                 */ 
                unsigned getCardinal() const { return m_cardinal; }

                /** Returns the maximum dimension, that is the highest coordinate which is in the node.
                 */ 
                unsigned int getMaxDimension() const { return m_dimension; }

                /** Returns the weight of the projection represented by the node.
                 */ 
                double getWeight() const { return m_weight; }

                /** Returns the maximum of the merits of the subprojections.
                 */ 
                unsigned int getMaxMeritsSubProj() const { return m_maxMeritsSubProj; }

                /** Returns the stored merit of the projection represented by the node.
                 */ 
                unsigned int getMeritMem() const {return m_meritMem; }

                /** Returns the temporary merit of the projection represented by the node.
                 */ 
                unsigned int getMeritTmp() const {return m_meritTmp; }

                /** Returns a pointer to the next projection to evaluate. May be null.
                 */ 
                Node* getNextNode() const { return m_nextNode; }

                /** Returns a vector of pointers to the subprojections whose cardinal is one less than the
                 * projection's.
                 */ 
                std::vector<Node*> getMotherNodes() const { return m_mothersNodes; }

                /** Returns the bitset representation of the projection represented by the node.
                 */ 
                Coordinates getProjectionRepresentation() const { 
                    Coordinates res;
                    accumulateProjectionRepresentation(res);
                    return res;
                }

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
                    m_maxMeritsSubProj = 0;
                    for (auto const* m : m_mothersNodes)
                    {
                        if (m->getMaxDimension() < m_dimension)
                        {
                            m_maxMeritsSubProj = std::max(m->getMeritMem(),m_maxMeritsSubProj);
                        }
                        else{
                            m_maxMeritsSubProj = std::max(m->getMeritTmp(),m_maxMeritsSubProj);
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
                unsigned int m_dimension; // last dimension (highest coordinate in the projection)
                unsigned int m_cardinal; // cardinal of the projection

                Node* m_nextNode = nullptr; // pointer to the next projection to evaluate
                std::vector<Node*> m_mothersNodes; // pointers to the subprojections whose cardinal is one less
                unsigned int m_maxMeritsSubProj = 0; // maximum of the merits of the subprojections
                unsigned int m_meritMem = 0; // stored merit
                unsigned int m_meritTmp = 0; // temporay merit

                void accumulateProjectionRepresentation(Coordinates& projection) const
                {
                    projection.insert(m_dimension-1);
                    if(m_cardinal>1)
                    {
                        m_mothersNodes[0]->accumulateProjectionRepresentation(projection);
                    }
                }
        };

        /** Comparison between pointers to nodes by dereferencing. Used to sort by decreasing importance.
         */ 
        static bool compareNodePointers(const Node* a, const Node* b){
            return (*a > * b);
        }

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
            double weight = m_figure->weights().getWeight(proj1DRep);
            Node* proj1D = new Node(m_dimension, proj1DRep.size(), weight);

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
                {   if (it->getCardinal() <= m_maxCardinal-1)
                    {
                        Coordinates projectionRep = it->getProjectionRepresentation(); // consider the projection
                        projectionRep.insert(m_dimension-1);
                        double weight =  m_figure->weights().getWeight(projectionRep);
                        Node* newNode = new Node(m_dimension, projectionRep.size() , weight); // create the node
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

            for (Node* node : newNodes) // for each new nodes
            {   
                Coordinates tmp = node->getProjectionRepresentation();
                for(int i = 0; i < m_dimension-1; ++i) // link to mothers which contains m_dimension
                {
                    if (tmp.find(i) != tmp.end())
                    {
                        //std::cout << tmp << " contains ";
                        tmp.erase(i);
                        auto foo = mapsToNodes.find(tmp)->second;
                        //std::cout << foo->getProjectionRepresentation() << std::endl;
                        node->addMother(foo);
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
            for(int i = 1; i <= m_dimension; ++i)
            {
                saveMerits(i);
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

        virtual void reset() { invalidate(1); }

        int m_dimension; // last dimension of J_d
        int m_maxCardinal; // maximal cardinality of projections to consider (alpha)
        std::vector<Node*> m_roots; // pointer to the first node to evaluate
        std::vector<bool> m_validFlags;

};

template<>
class WeightedFigureOfMerit<TValueProjMerit<NetEmbed::EMBEDDED>>::WeightedFigureOfMeritEvaluator : public FigureOfMeritEvaluator
{
    public:

        WeightedFigureOfMeritEvaluator(WeightedFigureOfMerit* figure):
                    m_figure(figure),
                    m_dimension(0),
                    m_maxCardinal(m_figure->projDepMerit().maxCardinal())
        {};

        ~WeightedFigureOfMeritEvaluator()
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

        virtual MeritValue operator() (const DigitalNet& net, unsigned int dimension, MeritValue initialValue, bool verbose = false)
        {

            extendUpToDimension(dimension);

            assert(isValid(dimension-1)); // check that the previous dimensions were correctly computed
            assert(net.dimension()>=dimension);
            invalidate(dimension + 1); // declare invalid the merits stored for upper dimensions

            auto acc = m_figure->accumulator(std::move(initialValue));

            Node* it = m_roots[dimension-1]; // iterator over the nodes
            do
            {   
                Real weight = it->getWeight();

                it->updateMaxMeritsSubProj()
                ;
                std::vector<unsigned int> meritEmbedded = m_figure->projDepMerit()(net,it->getProjectionRepresentation(),it->getMaxMeritsSubProj()); // compute the merit of the projection
                
                Real merit = m_figure->projDepMerit().combine(meritEmbedded);

                it->setMeritTmp(std::move(meritEmbedded)); // update the merit of the node
                
                if(verbose)
                {
                    std::cout << *it << " - merit: " << merit << std::endl;
                }

                acc.accumulate(weight,merit,1);

                if (!onProgress()(acc.value())) 
                {
                    acc.accumulate(std::numeric_limits<Real>::infinity(), merit, 1);
                    onAbort()(net);
                    validate(dimension);
                    return acc.value();
                }
                it = it->getNextNode(); // skip to next node
            }
            while(it != nullptr);

            validate(dimension);
            saveMerits(dimension);
            return acc.value();
    }

    private:
        WeightedFigureOfMerit* m_figure;

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
                Node(unsigned int dimension, unsigned int cardinal, double weight):
                    m_weight(weight),
                    m_cardinal(cardinal),
                    m_dimension(dimension)
                {};

                /** Returns the cardinal of the projection represented by the node.
                 */ 
                unsigned getCardinal() const { return m_cardinal; }

                /** Returns the maximum dimension, that is the highest coordinate which is in the node.
                 */ 
                unsigned int getMaxDimension() const { return m_dimension; }

                /** Returns the weight of the projection represented by the node.
                 */ 
                double getWeight() const { return m_weight; }

                /** Returns the maximum of the merits of the subprojections.
                 */ 
                std::vector<unsigned int> getMaxMeritsSubProj() const { return m_maxMeritsSubProj; }

                /** Returns the stored merit of the projection represented by the node.
                 */ 
                std::vector<unsigned int> getMeritMem() const {return m_meritMem; }

                /** Returns the temporary merit of the projection represented by the node.
                 */ 
                std::vector<unsigned int> getMeritTmp() const {return m_meritTmp; }

                /** Returns a pointer to the next projection to evaluate. May be null.
                 */ 
                Node* getNextNode() const { return m_nextNode; }

                /** Returns a vector of pointers to the subprojections whose cardinal is one less than the
                 * projection's.
                 */ 
                std::vector<Node*> getMotherNodes() const { return m_mothersNodes; }

                /** Returns the bitset representation of the projection represented by the node.
                 */ 
                Coordinates getProjectionRepresentation() const { 
                    Coordinates res;
                    accumulateProjectionRepresentation(res);
                    return res;
                }

                /** Set the temporary merit of the node to be the given merit
                 * @param merit is the merit to assign to the node.
                 */ 
                void setMeritTmp(const std::vector<unsigned int>& merit) {m_meritTmp = merit; }

                void setMeritTmp(std::vector<unsigned int>&& merit) {m_meritTmp = std::move(merit); }

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
                    std::fill(m_maxMeritsSubProj.begin(), m_maxMeritsSubProj.end(), 0);
                    for (auto const* m : m_mothersNodes)
                    {
                        if (m->getMaxDimension() < m_dimension)
                        {
                            std::transform(m_maxMeritsSubProj.begin(),m_maxMeritsSubProj.end(),
                                           m->getMeritMem().begin(), std::back_inserter(m_maxMeritsSubProj), std::greater<unsigned int> {});
                        }
                        else{
                            std::transform(m_maxMeritsSubProj.begin(),m_maxMeritsSubProj.end(),
                                           m->getMeritTmp().begin(), std::back_inserter(m_maxMeritsSubProj), std::greater<unsigned int> {});
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
                unsigned int m_dimension; // last dimension (highest coordinate in the projection)
                unsigned int m_cardinal; // cardinal of the projection

                Node* m_nextNode = nullptr; // pointer to the next projection to evaluate
                std::vector<Node*> m_mothersNodes; // pointers to the subprojections whose cardinal is one less
                std::vector<unsigned int> m_maxMeritsSubProj; // maximum of the merits of the subprojections
                std::vector<unsigned int> m_meritMem; // stored merit
                std::vector<unsigned int> m_meritTmp; // temporay merit

                void accumulateProjectionRepresentation(Coordinates& projection) const
                {
                    projection.insert(m_dimension-1);
                    if(m_cardinal>1)
                    {
                        m_mothersNodes[0]->accumulateProjectionRepresentation(projection);
                    }
                }
        };

        /** Comparison between pointers to nodes by dereferencing. Used to sort by decreasing importance.
         */ 
        static bool compareNodePointers(const Node* a, const Node* b){
            return (*a > * b);
        }

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
            double weight = m_figure->weights().getWeight(proj1DRep);
            Node* proj1D = new Node(m_dimension, proj1DRep.size(), weight);

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
                {   if (it->getCardinal() <= m_maxCardinal-1)
                    {
                        Coordinates projectionRep = it->getProjectionRepresentation(); // consider the projection
                        projectionRep.insert(m_dimension-1);
                        double weight =  m_figure->weights().getWeight(projectionRep);
                        Node* newNode = new Node(m_dimension, projectionRep.size() , weight); // create the node
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

            for (Node* node : newNodes) // for each new nodes
            {   
                Coordinates tmp = node->getProjectionRepresentation();
                for(int i = 0; i < m_dimension-1; ++i) // link to mothers which contains m_dimension
                {
                    if (tmp.find(i) != tmp.end())
                    {
                        //std::cout << tmp << " contains ";
                        tmp.erase(i);
                        auto foo = mapsToNodes.find(tmp)->second;
                        //std::cout << foo->getProjectionRepresentation() << std::endl;
                        node->addMother(foo);
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
            for(int i = 1; i <= m_dimension; ++i)
            {
                saveMerits(i);
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

        virtual void reset() { invalidate(1); }


        int m_dimension; // last dimension of J_d
        int m_maxCardinal; // maximal cardinality of projections to consider (alpha)
        std::vector<Node*> m_roots; // pointer to the first node to evaluate
        std::vector<bool> m_validFlags;

};

}}

#endif 

