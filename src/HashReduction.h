#ifndef  HASHREDUCTION_H
#define  HASHREDUCTION_H

#include <iostream>
#include <map>
#include "GPDDNode.h"
using namespace std;

struct HashR {
	GPDDNode* NodeInfo;
	HashR* next;
};

class HashReduction {
	private:
		const static int PRIME = 46153;
		typedef map<int, HashR*> HashTable;
		typedef HashTable::iterator it_HashTable;
		HashTable hTable;
		int HashFunc(const GPDDNode* node);
		bool Compare(const GPDDNode* node1, const GPDDNode* node2, const GPDDNode* Zero);
		int crashTime;
		int hitTime;
		int nodeNum;

	public:
		HashReduction();
		GPDDNode* insert(GPDDNode* node, bool &flag, const GPDDNode* Zero);
		void clean();
		void Print() const;
		inline int TimesOfHit() const {return hitTime;}
		inline int TimesOfCrash() const {return crashTime;}
		inline int KeyNum() const {return hTable.size();}
		inline int NodeNum() const {return nodeNum;}
		~HashReduction();
};

#endif  /*HASHGREDUCTION_H*/
