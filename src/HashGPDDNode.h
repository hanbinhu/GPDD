#ifndef  HASHGPDDNODE_H
#define  HASHGPDDNODE_H

#include <iostream>
#include <map>
#include "GPDDNode.h"
using namespace std;

struct HashNode {
	GPDDNode* NodeInfo;
	HashNode* next;
};

class HashGPDDNode {
	private:
		const static int PRIME = 2024327;
		typedef map<int, HashNode*> HashTable;
		typedef HashTable::iterator it_HashTable;
		HashTable hTable;
		int HashFunc(const GPDDNode* node);
		bool Compare(const GPDDNode* node1, const GPDDNode* node2);
		int crashTime;
		int hitTime;
		int nodeNum;

	public:
		HashGPDDNode();
		GPDDNode* insert(GPDDNode* node);
		void Print() const;
		inline int TimesOfHit() const {return hitTime;}
		inline int TimesOfCrash() const {return crashTime;}
		inline int KeyNum() const {return hTable.size();}
		inline int NodeNum() const {return nodeNum;}
		~HashGPDDNode();
};

#endif  /*HASHGPDDNODE_H*/
