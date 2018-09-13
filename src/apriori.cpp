/* ===========================================================================
   $File: apriori.cpp $
   $Date: 12 September 2018. $
   $Revision: $
   $Author: Nikita Smirnov $
   =========================================================================== */
#include <algorithm>
#include <iostream>
#include <cassert>
#include <cmath>

#include "defs.h"
#include "apriori.h"

namespace apriori {
	AprioriAlg::AprioriAlg(const Matrix &inputData, double minSupport, uint32 maxItems) :
		m_currentStep(0),
		m_subsetCounter(1),
		m_minRelSupport(minSupport),
		m_transactions(inputData)
	{
		m_root = new Nodes();
		m_root->reserve(maxItems);
		m_nTransactions = m_transactions.size();
		m_minAbsSupport = std::round(minSupport * m_nTransactions * 0.01);
	}

	void AprioriAlg::runAlg() {
		
		while (true) {
			m_currentStep++;
			// NOTE: Impossible to progress further
			// Gather frequent sets and finish algorithm
			if (m_subsetCounter < m_currentStep) {
				m_frequentSetsNumber = List(m_currentStep, 0);
				m_allFrequentSets = new Frequents(m_currentStep);
				getFrequentSets(m_root, nullptr, 0);
				generateRules();
				break;
			}
			m_subsetCounter = 0;

			// NOTE: Main loop which is finding out frequent sets
			for (auto &transaction : m_transactions) {
				List items(transaction);
				traverseTree(m_root, &items, m_currentStep);
			}
		}
	}

	void AprioriAlg::getFrequentSets(Nodes *&root, const List *passedSets, int k) {
		List passed;
		if (passedSets) {
			passed = List(*passedSets);
		}
		// NOTE: Root node, start traversing from here
		// First we are checking k-itemsets
		for (auto &set : *root) {
			if (set.second->support < m_minAbsSupport) {
				continue;
			}
			m_frequentSetsNumber[k]++;

			// NOTE: If set has passed minSupport add it to list
			passed.push_back(set.first);

			// NOTE: Save set as a frequent one
			Frequent *f = new Frequent();
			f->support = set.second->support;
			f->items = List(passed.begin(), passed.end());
			(*m_allFrequentSets)[passed.size()].push_back(f);

			// NOTE: Go deeper, check k+1-itemsets
			getFrequentSets(set.second->children, &passed, k + 1);

			// NOTE: Remove items on each iteration after climbing up
			passed.pop_back();
		}
	}

	void AprioriAlg::traverseTree(Nodes *&root, const List *items, int depth) {
		depth--;
		if (depth == 0) {
			// NOTE: Leaf node, start traversing through items
			// to look for new or existing nodes
			for (auto &item : *items) {
				auto treeItem = root->find(item);
				if (treeItem == root->end()) {
					m_subsetCounter++;
					// NOTE: Need to add new node
					Node *child = new Node();
					child->support = 1;
					child->children = new Nodes();
					(*root)[item] = child;
				} else {
					// NOTE: Already have this item, increase support value
					treeItem->second->support++;
				}
			}
		} else {
			// NOTE: Internal nodes, try to go deeper
			// TODO,STUDY: Probably possible to improve by filtering items 
			// with support < minSupport before going down the tree?
			auto iter = items->begin();
			for (; iter != items->end(); iter++) {
				auto item = *iter;
				if ((*root)[item]->support < m_minAbsSupport) {
					// NOTE: Ignoring items with support < minSupport
					continue;
				} else {
					// NOTE: Internal node with high support
					// Traverse further down if we have more items
					uint32 i = std::distance(items->begin(), iter);
					if (items->size() - i > 1) {
						auto newItems = List(items->begin() + i + 1, items->end());
						traverseTree((*root)[item]->children, &newItems, depth);
					}
				}
			} 
		}
	}

	void AprioriAlg::generateRules() {
		// NOTE: Start from the 3rd element (k=2)
		// For rules we need to have at least two items
		// So we only interested in k-itemsets with k>=2
		// NOTE: Almost the same process as traverseTree
		auto depthIter = m_allFrequentSets->begin() + 2;
		for (; depthIter != m_allFrequentSets->end(); depthIter++){
			for (auto &freqSet : *depthIter) {
				List items(freqSet->items);
				generateRules(m_root, freqSet->support, &items, items, nullptr);
			}
		}
	}

	void AprioriAlg::generateRules(Nodes *&root, int support, const List *currentItems,
	                               const List& allItems, const List* xSetItems)
	{
		auto iter = currentItems->begin();
		for (; iter != currentItems->end(); iter++) {
			uint32 itemId = *iter;
			List xSet;
			if (xSetItems) {
				xSet = List(*xSetItems);
			}
			// NOTE: Recursively fill X-itemset
			xSet.push_back(itemId);
			auto item = root->find(itemId);

			// NOTE: All items from frequent set is in X-set?
			// Not a useful rule
			if (xSet.size() == allItems.size()) {
				break;
			}

			// NOTE: Construct Y-set from allItems and X-set
			List ySet;
			std::set_difference(allItems.begin(), allItems.end(),
			                    xSet.begin(), xSet.end(),
			                    std::back_inserter(ySet));
			AssociationRule rule;
			rule.relSupport = (double)item->second->support / (double)m_nTransactions;
			rule.confidence = (double)support / (double)item->second->support;
			rule.x = List(xSet);
			rule.y = List(ySet);
			m_rules.push_back(rule);
			uint32 i = std::distance(currentItems->begin(), iter);
			if (currentItems->size() - i > 1) {
				auto newItems = List(currentItems->begin() + i + 1, currentItems->end());
				generateRules((*root)[itemId]->children, support,
				              &newItems, allItems, &xSet);
			}
		}
	}

	void AprioriAlg::printRules(uint32 nRules, Order order) {
		uint32 iRules = 0;
		if (order == Order::ASC) {
			std::sort(m_rules.begin(), m_rules.end(),
			          [](const AssociationRule &lhs, const AssociationRule &rhs){
				          return lhs.relSupport < rhs.relSupport;
			          });
		} else if (order == Order::DESC) {
			std::sort(m_rules.begin(), m_rules.end(),
			          [](const AssociationRule &lhs, const AssociationRule &rhs){
				          return lhs.relSupport > rhs.relSupport;
			          });
		}
		for (auto &rule : m_rules) {
			if (iRules > nRules) {
				break;
			}
			std::string xSet = vecToStr(rule.x);
			std::string ySet = vecToStr(rule.y);

			std::cout << xSet << " -> " << ySet;
		
			std::cout << " (" << rule.relSupport
			          << ", " << rule.confidence << ")" << std::endl;
			iRules++;
		}		
	}
}
