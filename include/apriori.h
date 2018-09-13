/* ===========================================================================
   $File: apriori.h $
   $Date: 12 September 2018. $
   $Revision: $
   $Author: Nikita Smirnov $
   =========================================================================== */

#ifndef APRIORI_H
#define APRIORI_H

#include <unordered_map>
#include <set>
#include "defs.h"

namespace apriori {
	using List = std::vector<uint32>;
	using Matrix = std::vector<List>;

	struct Frequent {
		List items;
		uint32 support;
	};
	using Frequents = std::vector<std::vector<Frequent*>>;

	struct AssociationRule {
		List x;
		List y;
		double setSupport;
		double relSupport;
		double confidence;
	};
	using Rules = std::vector<AssociationRule>;

	struct Node {
		uint32 support;
		std::unordered_map<uint32, Node*> *children;
	};

	using Nodes = std::unordered_map<uint32, Node*>;

	class AprioriAlg {

	private:
		uint32 m_currentStep;
		uint32 m_subsetCounter;
		uint32 m_minAbsSupport;
		uint32 m_nTransactions;
		double m_minRelSupport;
		Matrix m_transactions;

		Nodes *m_root; 
		
		Frequents *m_allFrequentSets;
		Rules m_rules;
		List m_frequentSetsNumber;

		void traverseTree(Nodes *&root, const List *items, int depth);
		void getFrequentSets(Nodes *&root, const List *seen, int k);
		// NOTE: First method initializes rules generation
		// Second is used to traverse tree
		void generateRules();
		void generateRules(Nodes*&, int, const List*, const List&, const List*);

	public:
		AprioriAlg(const Matrix &inputData, double minSupport, uint32 maxItems);
		void runAlg();
		void printRules(uint32 nRules, Order order);
	};
}
#endif
