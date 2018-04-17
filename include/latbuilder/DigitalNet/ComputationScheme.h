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

        /** Constructs of a computation scheme for a figure of merits based on the t-value of subjprojections.
         * @param lastDimension is the last dimension added to the net
         * @param maximalCardinality is the maximum cardinality of subprojections to consider
         * @param weights is TO CLARIFY
         */  
        ComputationScheme(int lastDimension, int maximalCardinality, WEIGHTS weights):
        m_lastDimension(lastDimension),
        m_maximalCardinality(maximalCardinality),
        m_weights(weights)
        {
            //assertions that we do not intend to create an empty network
            assert(lastDimension >= 2);
            assert(maximalCardinality >= 2);

            // creating the 2D projections
            std::vector<Node*> projections;
            for(int i = 0; i < lastDimension - 1; ++i){
                projection tmp = projection(lastDimension).flip(i).flip(lastDimension-1);
                double weight = weights(tmp);
                projections.push_back(new Node(std::move(tmp),lastDimension,weight));
            }

            // sorting the 2D projections by decreasing weights
            std::sort(projections.begin(),projections.end(),compareNodePointers);
            
            for(int i = 0; i < projections.size() - 1; ++i){
                projections[i]->setNextNode(projections[i+1]);
            }

            m_root = projections.front();

            int card = 2;
            std::map<projection,std::set<int>> seen;
            std::vector<Node*> newProjections;
            //creating the bigger projections
            while (card<maximalCardinality)
            {
                newProjections.clear();
                seen.clear();
                for(int i = 0; i < projections.size(); ++i)
                {
                    for(int j = i + 1; j < projections.size(); ++j)
                    {
                        projection newProjRep = projections[i]->getProjectionRepresentation() | projections[j]->getProjectionRepresentation();
                        int new_card = newProjRep.count();
                        if (new_card == card+1)
                        {
                            auto it = seen.find(newProjRep);
                            if (it != seen.end()){
                                it->second.insert(i);
                                it->second.insert(j);
                            }
                            else
                            {
                                std::set<int> tmp;
                                tmp.insert(i);
                                tmp.insert(j);
                                seen[newProjRep] = std::move(tmp);
                                double weight = weights(newProjRep);
                                newProjections.push_back(new Node(std::move(newProjRep),lastDimension,weight));
                            }
                        }
                    }
                }
                std::sort(newProjections.begin(), newProjections.end(), compareNodePointers);
                for (auto* proj : newProjections)
                {
                    std::vector<Node*> mothers;
                    for(const auto& idx : seen[proj->getProjectionRepresentation()])
                    {
                        mothers.push_back(projections[idx]);
                    }
                    proj->setMotherNodes(std::move(mothers));
                }
                for(int i = 0; i < newProjections.size()-1; ++i){
                    newProjections[i]->setNextNode(newProjections[i+1]);
                }
                projections.back()->setNextNode(newProjections.front());
                projections = std::move(newProjections);
                ++card;
            }
        };

        /** Constructs of a computation scheme for a figure of merits based on the t-value of subjprojections.
         * This constructor use a computation scheme whose last dimension is one less to get the t-values of the subprojections
         * which are not is the current computation scheme.
         * @param lastDimension is the last dimension added to the net
         * @param maximalCardinality is the maximum cardinality of subprojections to consider
         * @param weights is TO CLARIFY
         */ 
        ComputationScheme(int lastDimension, int maximalCardinality, WEIGHTS weights, const ComputationScheme& previousProjections):
        m_lastDimension(lastDimension),
        m_maximalCardinality(maximalCardinality),
        m_weights(weights)
        {
            //assertions that we do not intend to create an empty network
            assert(lastDimension >= 2);
            assert(maximalCardinality >= 2);

            //recover the lower bound for t-value from previous projections
            std::map<projection,int> lowerBoundspreviousProjections;
            previousProjections.getAllTValues(lowerBoundspreviousProjections);

            // creating the 2D projections
            std::vector<Node*> projections;
            for(int i = 0; i < lastDimension - 1; ++i){
                projection tmp = projection(lastDimension).flip(i).flip(lastDimension-1);
                double weight = weights(tmp);
                projections.push_back(new Node(std::move(tmp),lastDimension,weight));
            }

            // sorting the 2D projections by decreasing weights
            std::sort(projections.begin(),projections.end(),compareNodePointers);
            
            for(int i = 0; i < projections.size() - 1; ++i){
                projections[i]->setNextNode(projections[i+1]);
            }

            m_root = projections.front();

            int card = 2;
            std::map<projection,std::set<int>> seen;
            std::vector<Node*> newProjections;
            //creating the bigger projections
            while (card<maximalCardinality)
            {
                newProjections.clear();
                seen.clear();
                for(int i = 0; i < projections.size(); ++i)
                {
                    for(int j = i + 1; j < projections.size(); ++j)
                    {
                        projection newProjRep = projections[i]->getProjectionRepresentation() | projections[j]->getProjectionRepresentation();
                        int new_card = newProjRep.count();
                        if (new_card == card+1)
                        {
                            auto it = seen.find(newProjRep);
                            if (it != seen.end()){
                                it->second.insert(i);
                                it->second.insert(j);
                            }
                            else
                            {
                                std::set<int> tmp;
                                tmp.insert(i);
                                tmp.insert(j);
                                seen[newProjRep] = std::move(tmp);
                                double weight = weights(newProjRep);
                                
                                int lowerBound = 0;
                                projection resizedNewProjRep = newProjRep;
                                resizedNewProjRep.resize(lastDimension-1);
                                for( auto& kv : lowerBoundspreviousProjections)
                                {
                                    auto k = kv.first;
                                    if ((resizedNewProjRep - kv.first).empty())
                                    {
                                        lowerBound = std::min(kv.second,lowerBound);
                                    }
                                }

                                newProjections.push_back(new Node(std::move(newProjRep),lastDimension,weight,lowerBound));
                            }
                        }
                    }
                }
                std::sort(newProjections.begin(), newProjections.end(), compareNodePointers);
                for (auto* proj : newProjections)
                {
                    std::vector<Node*> mothers;
                    for(const auto& idx : seen[proj->getProjectionRepresentation()])
                    {
                        mothers.push_back(projections[idx]);
                    }
                    proj->setMotherNodes(std::move(mothers));
                }
                for(int i = 0; i < newProjections.size()-1; ++i){
                    newProjections[i]->setNextNode(newProjections[i+1]);
                }
                projections.back()->setNextNode(newProjections.front());
                projections = std::move(newProjections);
                ++card;
            }
        };

        /** Destructs the computation scheme/
         */ 
        ~ComputationScheme()
        {
            delete m_root;
        }

        /** Add to the map passed by reference the t-value of all the projections in the scheme for the best network so far.
         * The keys are bitset representation of the projections.
         * @param tValuesMap is a map to contain the tvalues.
         */ 
        void getAllTValues(std::map<projection,int>& tValuesMap) const{
            const Node* it = m_root;
            do
            {
                tValuesMap.insert(std::pair<projection,int>(it->getProjectionRepresentation(),it->getTValueBestNet()));
                it = it->getNextNode();
            }
            while(it != NULL);
        }

        /** Compute a figure of merit on the given net, update the pointer to the best net and the optimal figure of merit
         * if required.
         * @param net is a reference to the net to consider.
         * @param bestNet is a reference to a pointer to the best net so far.
         * @param optimalFigureOfMerit is a reference to the best figure of merit so far.
         */ 
        template <typename DERIVED, uInteger BASE>
        void computeFigureOfMerit(const DigitalNet<DERIVED,BASE>& net, DigitalNet<DERIVED,BASE>*& bestNet, double& optimalFigureOfMerit)
        {
            typedef typename DigitalNet<DERIVED,BASE>::GeneratingMatrix GeneratingMatrix;
            double acc = 0; // accumulatof for the figure of merit
            Node* it = m_root; // iterator over the nodes
            do
            {   
                double weight = it->getWeight();
                std::vector<GeneratingMatrix> genMatrices;
                for(int i = 0; i < m_lastDimension-1; ++i)
                {
                    if (it->getProjectionRepresentation()[i]){
                        genMatrices.push_back(net.generatingMatrix(i+1)); // compute the generating matrices
                    }
                }
                genMatrices.push_back(net.generatingMatrix(m_lastDimension));

                int tValue = COMPUTATION_METHOD::computeTValue(std::move(genMatrices),it->getLowerBound()); // compute the t-value of the projection
                std::cout << *it << " - t-value: " << tValue << std::endl;
                FIGURE_OF_MERIT::updateFigure(acc,tValue,weight);
                it->setTValueCurrentNet(tValue); // update the t-value of the node
                it = it->getNextNode(); // skip to next node
            }
            while(it != NULL);
            if (acc < optimalFigureOfMerit)
            {
                // update the references and the t-values for the best net if required.
                *bestNet = net;
                optimalFigureOfMerit = acc;
                setTValuesBestNet();
            }
        }

        /** Set the t-values for the best net of all the nodes to be the t-values of the current net.
         */  
        void setTValuesBestNet()
        {
            Node* it = m_root;
            do
            {
                it->setTValueBestNet();
                it = it->getNextNode();
            }
            while(it != NULL);
        }

        /** Overloads the << operator to print computation scheme.
         */ 
        friend std::ostream& operator<<(std::ostream& os, const ComputationScheme& dt)
            {
                Node* it = dt.m_root;
                do
                {
                    std::cout << *it << std::endl;
                    it = it->getNextNode();
                }
                while(it != NULL);
                return os;
            }   

    private:

        /** Private class to represent each element of \fJ_d\f. 
         */ 
        class Node
        {

            public:
                /** Constructor for class Node.
                 * @param projRep is the bitset representation of the projection
                 * @param lastDimension is the equivalent of \fd\f
                 * @param weight is the importance of the projection in the figure of merit
                 * @param lowerBoundPreviousProjections is a lower bound on the t-value of the projection. 
                 * Most often it is the maximum of the t-values of the subprojections.
                 */ 
                Node(const projection& projRep, int lastDimension, double weight, int lowerBoundPreviousProjections = 0):
                    m_weight(weight),
                    m_projRep(projRep),
                    m_lastDimension(lastDimension),
                    m_lowerBoundPreviousProjections(lowerBoundPreviousProjections)
                {
                    assert(projRep.size()==lastDimension);
                    m_cardinal = projRep.count();
                }

                ~Node()
                {
                    delete m_nextNode; // recursive destruction of the structure
                }

                /** Returns the cardinal of the projection represented by the node.
                 */ 
                int getCardinal() const { return m_cardinal; }

                /** Returns the weight of the projection represented by the node.
                 */ 
                int getWeight() const { return m_weight; }

                /** Returns a lower bound on the t-value of the projection represented by the node.
                 */ 
                int getLowerBound() const { return m_lowerBound; }

                /** Returns the t-value of the projection represented by the node for the best net so far.
                 */ 
                int getTValueBestNet() const {return m_tValueBestNet; }

                /** Returns the t-value of the projection represented by the node for the current net.
                 */ 
                int getTValueCurrentNet() const {return m_tValueCurrentNet; }

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
                void setTValueCurrentNet(int tValue) {m_tValueCurrentNet = tValue; }

                /** Set the next node of the current node.
                 * @param node is a pointer to the next node.
                 */ 
                void setNextNode(Node* node){ m_nextNode = node; }

                /** Set the mother nodes of the current node.
                 * @param mothers is a vector of pointers to the mother nodes.
                 */ 
                void setMotherNodes(std::vector<Node*> mothers) { m_mothersNodes = mothers; }

                /** Set the t-value of the current projection for the best net so far to be the t-value for the current net.
                 * This should be used when a better net is found.
                 */ 
                void setTValueBestNet() {m_tValueBestNet = m_tValueCurrentNet; }

                /** Compute a lower bound on the t-value of the projection represented by the node. 
                 * Notice that this bound is based on the t-values of the mothers and on a private member variable which 
                 * represents an the maximum of the t-values of the subprojections of the current projection which are not in
                 * the computation scheme.
                 */ 
                void updateLowerBound(){
                    m_lowerBound = m_lowerBoundPreviousProjections;
                    for (auto const* m : m_mothersNodes)
                    {
                        m_lowerBound = std::min(m->getTValueCurrentNet(),m_lowerBound);
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
                    for (projection::size_type n = 0; n < dt.m_lastDimension-1; ++n){
                        if (dt.m_projRep.test(n)){
                            os << n+1 << ", ";
                        }
                    }
                    if (dt.m_projRep.test(dt.m_lastDimension-1))
                    {
                        os << dt.m_lastDimension;
                    }
                    os << "} - weight: " << dt.getWeight();
                    return os;
                }

            private:
                double m_weight; // weight of the projection
                projection m_projRep; // bitset representation of the projection
                int m_lastDimension; // last dimension of J_d
                int m_cardinal; // cardinal of the projection

                Node* m_nextNode = NULL; // pointer to the next projection to evaluate
                std::vector<Node*> m_mothersNodes; // pointers to the subprojections which are is the scheme
                int m_lowerBound = 0; // lower bound on the t-value of the projection
                int m_tValueBestNet = 0; // t-value of the projection for the best net so far
                int m_tValueCurrentNet = 0; // t-value of the projection for the current net
                int m_lowerBoundPreviousProjections; // maximum of the t-values of the projections no in the scheme 
                                                     // which where previously evaluated.
        };

        /** Comparison between pointers to nodes by dereferencing.
         */ 
        static bool compareNodePointers(const Node* a, const Node* b){
            return !(*a < * b);
        }

        int m_lastDimension; // last dimension of J_d
        int m_maximalCardinality; // maximal cardinality of projections to consider (alpha)
        WEIGHTS m_weights; // TO CLARIFY represent a way to compute weights for projections
        Node* m_root = NULL; // pointer to the first node to evaluate

         

};

}}

#endif 

