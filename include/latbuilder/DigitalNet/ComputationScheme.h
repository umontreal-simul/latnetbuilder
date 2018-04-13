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
typedef typename boost::dynamic_bitset<> bitset;
typedef typename LatBuilder::uInteger uInteger;


// ComputationScheme is a class thar organize the computation of figures of merits based on the t-values of subprojections for digital nets

template <typename WEIGHTS, typename COMPUTATION_METHOD>
class ComputationScheme
{
    private:

        class Node
        {

            public:

                Node(const bitset& projRep, int lastDimension, double weight, int lowerBoundPreviousProjections = 0):
                    m_weight(weight),
                    m_projRep(projRep),
                    m_lastDimension(lastDimension),
                    m_lowerBoundPreviousProjections(lowerBoundPreviousProjections)
                {
                    m_cardinal = projRep.count() + 1;
                }

                ~Node()
                {
                    delete m_nextNode;
                }

                int getCardinal() const { return m_cardinal; }

                int getWeight() const { return m_weight; }

                int getLowerBound() const { return m_lowerBound; }

                int getTValueBestNet() const {return m_tValueBestNet; }

                int getTValueCurrentNet() const {return m_tValueCurrentNet; }

                Node* getNextNode() const { return m_nextNode; }

                std::vector<Node*> getMotherNodes() const { return m_mothersNodes; }

                bitset getProjectionRepresentation() const { return m_projRep; }

                void setTValueCurrentNet(int tValue) {m_tValueCurrentNet = tValue; }

                void setNextNode(Node* node){ m_nextNode = node; }

                void setMotherNodes(std::vector<Node*> mothers) { m_mothersNodes = mothers; }

                void setTValueBestNet() {m_tValueBestNet = m_tValueCurrentNet; }

                void updateLowerBound(){
                    m_lowerBound = m_lowerBoundPreviousProjections;
                    for (auto const* m : m_mothersNodes)
                    {
                        m_lowerBound = std::min(m->getTValueCurrentNet(),m_lowerBound);
                    }
                }

                //std::vector<Node*> getMotherNodes() { return m_mothersNodes; }

                //Node* getNextNode() { return m_nextNode; }

                bool operator<(const Node &b) const
                {
                    return getCardinal() < b.getCardinal() || getWeight() < b.getWeight();
                }

                friend std::ostream& operator<<(std::ostream& os, const Node& dt)
                {
                    os << "{";
                    for (bitset::size_type n = 0; n < dt.m_lastDimension-1; ++n){
                        if (dt.m_projRep.test(n)){
                            os << n+1 << ", ";
                        }
                    }
                    os << dt.m_lastDimension << "} - " ;
                    os << dt.getProjectionRepresentation() << " - ";
                    os << dt.getWeight() << std::endl;
                    return os;
                }

            private:
                double m_weight;
                bitset m_projRep;
                int m_lastDimension;
                int m_cardinal;

                Node* m_nextNode = NULL;
                std::vector<Node*> m_mothersNodes;
                int m_lowerBound = 0;
                int m_tValueBestNet = 0;
                int m_tValueCurrentNet = 0;
                int m_lowerBoundPreviousProjections;
        };

        static bool compareNodePointers(const Node* a, const Node* b){
            return !(*a < * b);
        }

        int m_lastDimension;
        int m_maximalCardinality;
        WEIGHTS m_weights;
        Node* m_root = NULL;

    public:

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
                bitset tmp = bitset(lastDimension-1).flip(i);
                double weight = weights(tmp,lastDimension);
                projections.push_back(new Node(std::move(tmp),lastDimension,weight));
            }

            // sorting the 2D projections by decreasing weights
            std::sort(projections.begin(),projections.end(),compareNodePointers);
            
            for(int i = 0; i < projections.size() - 1; ++i){
                projections[i]->setNextNode(projections[i+1]);
            }

            m_root = projections.front();

            int card = 2;
            std::map<bitset,std::set<int>> seen;
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
                        bitset newProjRep = projections[i]->getProjectionRepresentation() | projections[j]->getProjectionRepresentation();
                        int new_card = newProjRep.count() + 1;
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
                                double weight = weights(newProjRep,lastDimension);
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

        ComputationScheme(int lastDimension, int maximalCardinality, WEIGHTS weights, const ComputationScheme& previousProjections):
        m_lastDimension(lastDimension),
        m_maximalCardinality(maximalCardinality),
        m_weights(weights)
        {
            //assertions that we do not intend to create an empty network
            assert(lastDimension >= 2);
            assert(maximalCardinality >= 2);

            //recover the lower bound for t-value from previous projections
            std::map<bitset,int> lowerBoundspreviousProjections;
            previousProjections.getAllTValues(lowerBoundspreviousProjections);

            // creating the 2D projections
            std::vector<Node*> projections;
            for(int i = 0; i < lastDimension - 1; ++i){
                bitset tmp = bitset(lastDimension-1).flip(i);
                double weight = weights(tmp,lastDimension);
                projections.push_back(new Node(std::move(tmp),lastDimension,weight));
            }

            // sorting the 2D projections by decreasing weights
            std::sort(projections.begin(),projections.end(),compareNodePointers);
            
            for(int i = 0; i < projections.size() - 1; ++i){
                projections[i]->setNextNode(projections[i+1]);
            }

            m_root = projections.front();

            int card = 2;
            std::map<bitset,std::set<int>> seen;
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
                        bitset newProjRep = projections[i]->getProjectionRepresentation() | projections[j]->getProjectionRepresentation();
                        int new_card = newProjRep.count() + 1;
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
                                double weight = weights(newProjRep,lastDimension);
                                
                                int lowerBound = 0;
                                bitset resizedNewProjRep = newProjRep;
                                resizedNewProjRep.resize(lastDimension-2);
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

        ~ComputationScheme()
        {
            delete m_root;
        }

        void getAllTValues(std::map<bitset,int>& tValuesMap) const{
            const Node* it = m_root;
            do
            {
                tValuesMap.insert(std::pair<bitset,int>(it->getProjectionRepresentation(),it->getTValueBestNet()));
                it = it->getNextNode();
            }
            while(it != NULL);
        }

        template <typename DERIVED, uInteger BASE>
        void computeFigureOfMerit(const DigitalNet<DERIVED,BASE>& net, DigitalNet<DERIVED,BASE>* &bestNet, double& optimalFigureOfMerit){
            double acc = 0;
            Node* it = m_root;
            do
                {   
                    double weight = it->getWeight();
                    it->updateLowerBound();
                    if (weight>0){
                        int upperLimit = COMPUTATION_METHOD::computeUpperLimit(acc,weight,optimalFigureOfMerit);
                        int tValue = COMPUTATION_METHOD::updateFigureOfMerit(net, acc, it->getProjectionRepresentation(), it->getLowerBound(), upperLimit);
                        it->setTValueCurrentNet(tValue);
                        if (acc>= optimalFigureOfMerit)
                        {
                            return;
                        }
                    }
                    else{
                        it->setTValueCurrentNet(it->getLowerBound());
                    }
                    it = it->getNextNode();
                }
                while(it != NULL);
                if (acc < optimalFigureOfMerit)
                {
                    bestNet = &net;
                    optimalFigureOfMerit = acc;
                    setTValuesBestNet();
                }
        }

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

};

}}

#endif 

