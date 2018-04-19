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

#ifndef PROJECTION_NETWORK_H
#define PROJECTION_NETWORK_H

#include <vector>
#include <map>
#include <set>
#include <boost/dynamic_bitset.hpp> 
#include "latbuilder/Util.h"
#include "latbuilder/DigitalNet/DigitalNet.h"

namespace LatBuilder { namespace DigitalNet {

/** Class organizing the computation of figure of merits base on the t-value of a subset \fJ_d\f of projections
 * where \f \left{ u \subset \{1,...,d\} : d \in u, |u| \leq \alpha \right} \f.
 */ 
template <typename WEIGHTS, typename COMPUTATION_METHOD, typename FIGURE_OF_MERIT>
class ComputationScheme
{
    public:


        typedef typename boost::dynamic_bitset<> projection; /**< type alias for boost::dynamic_bitset. 
        Represents subprojections of a given projections. A given projection of \fJ_d\f is represented by a bitset of size \fd-1\f. 
         * For instance, \f1 \in u\f iff the first bit  of the bitset is true.  */

        typedef typename LatBuilder::uInteger uInteger; /**< type alia for unsigned */

        /** Constructs of a computation scheme for a figure of merits based on the t-value of subprojections.
         * The computation scheme visits the subprojections in a specific order to reduce computations.
         * @param maxCardinal is the biggest cardinal of subprojections to consider (corresponds to \f \alpha \f)
         * @param weights is TO CLARIFY
         */ 
        ComputationScheme(unsigned int maxCardinal, WEIGHTS weights):
        m_maxCardinal(maxCardinal),
        m_weights(weights),
        m_dimension(1)
        {
            assert(maxCardinal>=1); // otherwise we consider no projections
            
            // create projection {1}
            projection tmp = projection(m_dimension).flip(0); 
            double weight = m_weights(tmp);
            Node* root = new Node(tmp,m_dimension,weight); // set the first root to be the first node
            m_roots.push_back(root);
        }

        /** Extend by one dimension the ComputationScheme. This creates new nodes corresponding to the new projections to consider
         * while evaluating figures of merits.
         */ 
        void extend(){
            ++m_dimension; // increase maximal dimension
            std::vector<Node*> newNodes; // to store new nodes

            std::map<projection,Node*> mapsToNodes; // map between new projections and new nodes

            // create projection {m_dimension} 
            projection proj1DRep(m_dimension);
            proj1DRep.flip(m_dimension-1);
            double weight = m_weights(proj1DRep);
            Node* proj1D = new Node(proj1DRep,m_dimension,weight);
            newNodes.push_back(proj1D);

            mapsToNodes.insert(std::pair<projection,Node*>(proj1DRep,proj1D));


            for(int i = 0; i < m_dimension-1; ++i) // for each previous dimensions
            {
                Node* it = m_roots[i];
                do
                {   if (it->getProjectionRepresentation().count() <=m_maxCardinal-1)
                    {
                        projection projectionRep = it->getProjectionRepresentation(); // consider the projection
                        projectionRep.resize(m_dimension); // increase the size
                        projectionRep.flip(m_dimension-1); // add m_dimension to the set
                        double weight = m_weights(projectionRep);
                        Node* newNode = new Node(projectionRep,m_dimension,weight); // create the node
                        newNode->addMother(it);
                        mapsToNodes.insert(std::pair<projection,Node*>(projectionRep,newNode));
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
                projection tmp = kv.first;
                for(int i = 0; i < m_dimension-1; ++i) // link to mothers which contains m_dimension
                {
                    if (tmp[i])
                    {
                        tmp.flip(i);
                        kv.second->addMother(mapsToNodes.find(tmp)->second);
                        tmp.flip(i);
                    }
                }
            }
        }

        /** Extend several times the ComputationSchemme. 
         * @param times is the number of repetitions
         * @see extend
         */ 
        void extend(unsigned int times)
        {
            for(int i = 0; i < times; ++i)
            {
                extend();
            }
        }

        /** Destructs the computation scheme/
         */ 
        ~ComputationScheme()
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

        /** Returns a map containing all the t-values stored in the nodes corresponding to the given dimension, 
         * that is those whose highest element is equal to dimension.
         * @param dimension is the dimension to consider
         * @return a map between projections and t-values
         */ 
        std::map<projection,int> getAllTValues(unsigned int dimension) const{
            std::map<projection,int> tValuesMap;
            const Node* it = m_roots[dimension-1];
            do
            {
                tValuesMap.insert(std::pair<projection,int>(it->getProjectionRepresentation(),it->getTValueMem()));
                it = it->getNextNode();
            }
            while(it != nullptr);
            return tValuesMap;
        }

        /** Compute a figure of merit on the given net, update the pointer to the best net and the optimal figure of merit
         * if required.
         * @param net is a reference to the net to consider.
         * @param dimension is the last dimension (highest dimension in in all the subprojections)
         * @param bestNet is a reference to a pointer to the best net so far.
         * @param optimalFigureOfMerit is a reference to the best figure of merit so far.
         */ 
/*         template <typename DERIVED, uInteger BASE>
        void computeFigureOfMerit(const DigitalNet<DERIVED,BASE>& net, unsigned int dimension, DigitalNet<DERIVED,BASE>*& bestNet, double& optimalFigureOfMerit)
        {
            typedef typename DigitalNet<DERIVED,BASE>::GeneratingMatrix GeneratingMatrix;
            double acc = 0; // accumulator for the figure of merit
            Node* it = m_roots[dimension-1]; // iterator over the nodes
            do
            {   
                double weight = it->getWeight();
                it->updateMaxTValuesSubProj();
    
                std::vector<GeneratingMatrix> genMatrices;
                for(int i = 0; i < it->getMaxDimension()-1; ++i)
                {
                    if (it->getProjectionRepresentation()[i]){
                        genMatrices.push_back(net.generatingMatrix(i+1)); // compute the generating matrices
                    }
                }
                genMatrices.push_back(net.generatingMatrix(it->getMaxDimension()));

                int tValue = COMPUTATION_METHOD::computeTValue(std::move(genMatrices),it->getMaxTValuesSubProj()); // compute the t-value of the projection
                std::cout << *it << "- t-value sub: " << it->getMaxTValuesSubProj() << " - t-value: " << tValue << std::endl;
                FIGURE_OF_MERIT::updateFigure(acc,tValue,weight);
                it->setTValueTmp(tValue); // update the t-value of the node
                it = it->getNextNode(); // skip to next node
            }
            while(it != nullptr);
            if (acc < optimalFigureOfMerit)
            {
                // update the references and the t-values for the best net if required.
                *bestNet = net;
                optimalFigureOfMerit = acc;
                saveTValues();
            }
        } 
        */

        /** Evaluate the figure of merit on the given net, storing the result in the accumulator. This method consider all
         * the subprojections which are in the scheme.
         * @param net is a constant reference to the DigitalNet we want to evaluate
         * @param is a reference to the double in which we want to store the result
         */ 
        template <typename DERIVED, uInteger BASE>
        void evaluateFigureOfMerit(const DigitalNet<DERIVED,BASE>& net, double& acc, bool verbose=false)
        {
            typedef typename DigitalNet<DERIVED,BASE>::GeneratingMatrix GeneratingMatrix;

            std::vector<GeneratingMatrix> allGeneratingMatrices;

            for (int dim = 1; dim <= m_dimension; ++dim) // for each dimension
            {
                allGeneratingMatrices.push_back(net.generatingMatrix(dim));

                Node* it = m_roots[dim-1]; // iterator over the nodes
                do
                {   
                    double weight = it->getWeight(); // compute the weight

                    // store the generating matrices corresponding to the projection
                    std::vector<GeneratingMatrix> genMatrices;
                    for(int i = 0; i < it->getMaxDimension(); ++i)
                    {
                        if (it->getProjectionRepresentation()[i]){ // is coordinate i+1 is in the projection
                            genMatrices.push_back(allGeneratingMatrices[i]); // add the corresponding matrix
                        }
                    }

                    it->updateMaxTValuesSubProj(); // compute the maximum of the t-values of the subprojections
                    int tValue = COMPUTATION_METHOD::computeTValue(std::move(genMatrices),it->getMaxTValuesSubProj(),verbose); // compute the t-value of the projection
                    
                    if(verbose)
                    {
                        std::cout << *it << " - max t-values sub: " << it->getMaxTValuesSubProj() << " - t-value: " << tValue << std::endl;
                    }  
                    
                    FIGURE_OF_MERIT::updateFigure(acc,tValue,weight); // update the accumulator according to the figure of merit
                    it->setTValueTmp(tValue); // update the t-value of the node
                    it->saveTValue(); // store the computed tValue
                    it = it->getNextNode(); // skip to next node
                }
                while(it != nullptr);
            }
        }

        /** Save the t-values of all the nodes corresponding to the given dimension.
         * @param dimension is the dimension to consider
         */  
        void saveTValues(unsigned int dimension)
        {
            Node* it = m_roots[dimension-1];
            do
            {
                it->saveTValue();
                it = it->getNextNode();
            }
            while(it != nullptr);
        }

        /** Save the t-values of all the nodes in the scheme.
         */ 
        void saveTValues()
        {
            for(int i = 0; i< m_dimension; ++i)
            {
                Node* it = m_roots[i];
                do
                {
                    it->saveTValue();
                    it = it->getNextNode();
                }                 
                while(it != nullptr);
            }
        }

        /** Print the nodes corresponding to the given dimension.
         */ 
        void print(unsigned int dimension)
        {
            Node* it = m_roots[dimension-1];
            do
            {
                std::cout << *it << std::endl;
                it = it->getNextNode();
            }
            while(it != nullptr);
        }

    private:

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
                Node(const projection& projRep, int dimension, double weight):
                    m_weight(weight),
                    m_projRep(projRep),
                    m_dimension(dimension)
                {
                    assert(projRep.size()==dimension); // check that bitset representation of the projection has the correct size
                    m_cardinal = projRep.count(); // compute the cardinal
                }

                /** Returns the cardinal of the projection represented by the node.
                 */ 
                int getCardinal() const { return m_cardinal; }

                /** Returns the maximum dimension, that is the highest coordinate which is in the node.
                 */ 
                int getMaxDimension() const { return m_dimension; }

                /** Returns the weight of the projection represented by the node.
                 */ 
                int getWeight() const { return m_weight; }

                /** Returns the maximum of the t-values of the subprojections.
                 */ 
                int getMaxTValuesSubProj() const { return m_maxTValuesSubProj; }

                /** Returns the stored t-value of the projection represented by the node.
                 */ 
                int getTValueMem() const {return m_tValueMem; }

                /** Returns the temporary t-value of the projection represented by the node.
                 */ 
                int getTValueTmp() const {return m_tValueTmp; }

                /** Returns a pointer to the next projection to evaluate. May be null.
                 */ 
                Node* getNextNode() const { return m_nextNode; }

                /** Returns a vector of pointers to the subprojections whose cardinal is one less than the
                 * projection's.
                 */ 
                std::vector<Node*> getMotherNodes() const { return m_mothersNodes; }

                /** Returns the bitset representation of the projection represented by the node.
                 */ 
                projection getProjectionRepresentation() const { return m_projRep; }

                /** Set the temporary t-value of the node to be the given t-value
                 * @param tValue is the t-value to assign to the node.
                 */ 
                void setTValueTmp(int tValue) {m_tValueTmp = tValue; }

                /** Set the next node of the current node.
                 * @param node is a pointer to the next node.
                 */ 
                void setNextNode(Node* node){ m_nextNode = node; }

                void addMother(Node* mother) { m_mothersNodes.push_back(mother); }

                /** Save the temporary t-value in the m_tValueMem field. 
                 */ 
                void saveTValue() {m_tValueMem = m_tValueTmp; }

                /** Update the maximum of the t-values of the subprojections. Note that for
                 * subprojections which also contain getMaxDimension(), the temporary t-value is used
                 * whereas for other nodes, the stored t-value is used. This allows component-by-component
                 * evaluation for several nets using only one ComputationScheme.
                 */ 
                void updateMaxTValuesSubProj()
                {
                    m_maxTValuesSubProj=0;
                    for (auto const* m : m_mothersNodes)
                    {
                        if (m->getProjectionRepresentation()[m_dimension-1])
                        {
                            m_maxTValuesSubProj = std::max(m->getTValueTmp(),m_maxTValuesSubProj);
                        }
                        else{
                            m_maxTValuesSubProj = std::max(m->getTValueMem(),m_maxTValuesSubProj);
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
                    os << "projection: " << "{";
                    for (projection::size_type n = 0; n < dt.m_dimension-1; ++n){
                        if (dt.m_projRep.test(n)){
                            os << n+1 << ", ";
                        }
                    }
                    if (dt.m_projRep.test(dt.m_dimension-1))
                    {
                        os << dt.m_dimension;
                    }
                    os << "} - weight: " << dt.getWeight();

                    // block to also print mothers
                    /* os << std::endl << "    mothers: " << std::endl;
                    for(const auto& m: dt.getMotherNodes())
                    {
                        os << "      {";
                        for(projection::size_type n = 0; n < m->getMaxDimension()-1; ++n)
                        {
                            if (m->getProjectionRepresentation().test(n))
                            {
                                os << n+1 << ", ";
                            }  
                        }
                        if (m->getProjectionRepresentation().test(m->getMaxDimension()-1))
                        {
                            os << m->getMaxDimension();
                        }
                        os << "}" << std::endl; 
                    } */
                    return os;
                }

            private:
                double m_weight; // weight of the projection
                projection m_projRep; // bitset representation of the projection
                int m_dimension; // last dimension (highest coordinate in the projection)
                int m_cardinal; // cardinal of the projection

                Node* m_nextNode = nullptr; // pointer to the next projection to evaluate
                std::vector<Node*> m_mothersNodes; // pointers to the subprojections whose cardinal is one less
                int m_maxTValuesSubProj = 0; // maximum of the t-values of the subprojections
                int m_tValueMem = 0; // stored t-value
                int m_tValueTmp = 0; // temporay t-value
        };

        /** Comparison between pointers to nodes by dereferencing. Used to sort by decreasing importance.
         */ 
        static bool compareNodePointers(const Node* a, const Node* b){
            return (*a > * b);
        }

        int m_dimension; // last dimension of J_d
        int m_maxCardinal; // maximal cardinality of projections to consider (alpha)
        WEIGHTS m_weights; // TO CLARIFY represent a way to compute weights for projections
        std::vector<Node*> m_roots; // pointer to the first node to evaluate
};

}}

#endif 

