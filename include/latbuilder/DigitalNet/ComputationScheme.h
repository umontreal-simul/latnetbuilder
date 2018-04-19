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
         * @param maximalCardinality is the maximum cardinality of subprojections to consider
         * @param weights is TO CLARIFY
         */ 

        ComputationScheme(unsigned int maximalCardinality, WEIGHTS weights):
        m_maximalCardinality(maximalCardinality),
        m_weights(weights),
        m_dimension(1)
        {
            assert(maximalCardinality>=1);
            mapsToNodes.push_back(std::map<projection,Node*>());
            
            projection tmp = projection(m_dimension).flip(0);
            double weight = m_weights(tmp);
            Node* root = new Node(tmp,m_dimension,weight);
            mapsToNodes[0].insert(std::pair<projection,Node*>(tmp,root));
            m_roots.push_back(root);
        }

        void extend(){
            mapsToNodes.push_back(std::map<projection,Node*>());
            ++m_dimension;
            std::vector<Node*> newNodes;

            projection proj1DRep(m_dimension);
            proj1DRep.flip(m_dimension-1);
            double weight = m_weights(proj1DRep);
            Node* proj1D = new Node(proj1DRep,m_dimension,weight);
            newNodes.push_back(proj1D);
            mapsToNodes[m_dimension-1].insert(std::pair<projection,Node*>(proj1DRep,proj1D));

            for(int i = 0; i < m_dimension-1; ++i)
            {
                for (auto& kv : mapsToNodes[i])
                {
                    projection projectionRep = kv.first;
                    if (projectionRep.count() <= m_maximalCardinality-1)
                    {
                        
                        projectionRep.resize(m_dimension);
                        projectionRep.flip(m_dimension-1);
                        double weight = m_weights(projectionRep);
                        Node* newNode = new Node(projectionRep,m_dimension,weight);
                        newNode->addMother(kv.second);
                        mapsToNodes[m_dimension-1].insert(std::pair<projection,Node*>(projectionRep,newNode));
                        newNodes.push_back(newNode);
                    }
                } 
            }
            std::sort(newNodes.begin(),newNodes.end(),compareNodePointers);
            m_roots.push_back(newNodes[0]);
            for(int i = 0; i < newNodes.size()-1; ++i)
            {
                newNodes[i]->setNextNode(newNodes[i+1]);
            }
            for (auto& kv : mapsToNodes[m_dimension-1])
            {   
                projection tmp = kv.first;
                for(int i = 0; i < m_dimension-1; ++i)
                {
                    if (tmp[i])
                    {
                        tmp.flip(i);
                        kv.second->addMother(mapsToNodes[m_dimension-1].find(tmp)->second);
                        tmp.flip(i);
                    }
                }
            }
        }

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
            for (auto* root : m_roots)
            {
                delete root;
            }
        }

        /** Add to the map passed by reference the t-value of all the projections in the scheme for the best network so far.
         * The keys are bitset representation of the projections.
         * @param tValuesMap is a map to contain the tvalues.
         */ 
        std::map<projection,int> getAllTValues(unsigned int dimension) const{
            std::map<projection,int> tValuesMap;
            const Node* it = m_roots[dimension-1];
            do
            {
                tValuesMap.insert(std::pair<projection,int>(it->getProjectionRepresentation(),it->getTValueMem()));
                it = it->getNextNode();
            }
            while(it != NULL);
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
                it->computeMaxTValuesSubProj();
    
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
            while(it != NULL);
            if (acc < optimalFigureOfMerit)
            {
                // update the references and the t-values for the best net if required.
                *bestNet = net;
                optimalFigureOfMerit = acc;
                saveTValues();
            }
        } */

        template <typename DERIVED, uInteger BASE>
        void evaluateFigureOfMerit(const DigitalNet<DERIVED,BASE>& net, double& acc, bool verbose=false)
        {
            typedef typename DigitalNet<DERIVED,BASE>::GeneratingMatrix GeneratingMatrix;
            for (int dim = 1; dim <= m_dimension; ++dim)
            {
                Node* it = m_roots[dim-1]; // iterator over the nodes
                do
                {   
                    double weight = it->getWeight();
                    std::vector<GeneratingMatrix> genMatrices;
                    for(int i = 0; i < it->getMaxDimension()-1; ++i)
                    {
                        if (it->getProjectionRepresentation()[i]){
                            genMatrices.push_back(net.generatingMatrix(i+1)); // compute the generating matrices
                        }
                    }
                    genMatrices.push_back(net.generatingMatrix(it->getMaxDimension()));
                    it->computeMaxTValuesSubProj();
                    int tValue = COMPUTATION_METHOD::computeTValue(std::move(genMatrices),it->getMaxTValuesSubProj(), verbose); // compute the t-value of the projection
                    if (verbose){
                        std::cout << *it << " - max t-values sub: " << it->getMaxTValuesSubProj() << " - t-value: " << tValue << std::endl;
                    }
                    FIGURE_OF_MERIT::updateFigure(acc,tValue,weight);
                    it->setTValueTmp(tValue); // update the t-value of the node
                    it = it->getNextNode(); // skip to next node
                }
                while(it != NULL);
                saveTValues(dim);
            }
        }

        /** Save the t-values of all the nodes for the current net.
         */  
        void saveTValues(unsigned int dimension)
        {
            Node* it = m_roots[dimension-1];
            do
            {
                it->saveTValue();
                it = it->getNextNode();
            }
            while(it != NULL);
        }

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
                while(it != NULL);
            }
        }

        void print(unsigned int dimension)
        {
            Node* it = m_roots[dimension-1];
            do
            {
                std::cout << *it << std::endl;
                it = it->getNextNode();
            }
            while(it != NULL);
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
                 * @param lowerBoundPreviousProjections is a lower bound on the t-value of the projection. 
                 * Most often it is the maximum of the t-values of the subprojections.
                 */ 
                Node(const projection& projRep, int dimension, double weight):
                    m_weight(weight),
                    m_projRep(projRep),
                    m_dimension(dimension)
                {
                    assert(projRep.size()==dimension);
                    m_cardinal = projRep.count();
                }

                ~Node()
                {
                    delete m_nextNode; // recursive destruction of the structure
                }

                /** Returns the cardinal of the projection represented by the node.
                 */ 
                int getCardinal() const { return m_cardinal; }

                int getMaxDimension() const { return m_dimension; }

                /** Returns the weight of the projection represented by the node.
                 */ 
                int getWeight() const { return m_weight; }

                /** Returns a lower bound on the t-value of the projection represented by the node.
                 */ 
                int getMaxTValuesSubProj() const { return m_maxTValuesSubProj; }

                /** Returns the t-value of the projection represented by the node for the best net so far.
                 */ 
                int getTValueMem() const {return m_tValueMem; }

                /** Returns the t-value of the projection represented by the node for the current net.
                 */ 
                int getTValueTmp() const {return m_tValueTmp; }

                /** Returns a pointer to the next projection to evaluate.
                 */ 
                Node* getNextNode() const { return m_nextNode; }

                /** Returns a vector of pointers to the subprojections of the projection represented by the node
                 * which are in the same ComputationScheme.
                 */ 
                std::vector<Node*> getMotherNodes() const { return m_mothersNodes; }

                /** Returns the bitset representation of the projection represented by the node.
                 */ 
                projection getProjectionRepresentation() const { return m_projRep; }

                /** Set the t-value of the node to be the given t-value
                 * @param tValue is the t-value to assign to the node.
                 */ 
                void setTValueTmp(int tValue) {m_tValueTmp = tValue; }

                /** Set the next node of the current node.
                 * @param node is a pointer to the next node.
                 */ 
                void setNextNode(Node* node){ m_nextNode = node; }

                /** Set the mother nodes of the current node.
                 * @param mothers is a vector of pointers to the mother nodes.
                 */ 
                void setMotherNodes(std::vector<Node*> mothers) { m_mothersNodes = mothers; }

                void addMother(Node* mother) { m_mothersNodes.push_back(mother); }

                /** Set the t-value of the current projection for the best net so far to be the t-value for the current net.
                 * This should be used when a better net is found.
                 */ 
                void saveTValue() {m_tValueMem = m_tValueTmp; }

                /** Compute a lower bound on the t-value of the projection represented by the node. 
                 * Notice that this bound is based on the t-values of the mothers and on a private member variable which 
                 * represents an the maximum of the t-values of the subprojections of the current projection which are not in
                 * the computation scheme.
                 */ 
                void computeMaxTValuesSubProj()
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

                /** Overloading of the < operator to compare projections according to their weight 
                 * in the figure of merit. A projection is smaller than another one if they have the same cardinal and its weight is smaller
                 * of if it has a strictly bigger cardinal.
                 */ 
                bool operator<(const Node &b) const
                {
                    return (getCardinal() == b.getCardinal()) ? 
                                (getWeight() < b.getWeight()) : 
                                (getCardinal() > b.getCardinal());
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
                int m_dimension; // last dimension of J_d
                int m_cardinal; // cardinal of the projection

                Node* m_nextNode = NULL; // pointer to the next projection to evaluate
                std::vector<Node*> m_mothersNodes; // pointers to the subprojections which are is the scheme
                int m_maxTValuesSubProj = 0; // lower bound on the t-value of the projection
                int m_tValueMem = 0; // t-value of the projection for the best net so far
                int m_tValueTmp = 0; // t-value of the projection for the current net
                int m_maxTValuesSubProjPreviousProjections; // maximum of the t-values of the projections no in the scheme 
                                                     // which where previously evaluated.
        };

        /** Comparison between pointers to nodes by dereferencing.
         */ 
        static bool compareNodePointers(const Node* a, const Node* b){
            return !(*a < * b);
        }

        int m_dimension; // last dimension of J_d
        int m_maximalCardinality; // maximal cardinality of projections to consider (alpha)
        WEIGHTS m_weights; // TO CLARIFY represent a way to compute weights for projections
        std::vector<Node*> m_roots; // pointer to the first node to evaluate
        std::vector<std::map<projection, Node*>> mapsToNodes;
};

}}

#endif 

