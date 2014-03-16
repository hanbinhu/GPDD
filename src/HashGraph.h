#ifndef  HASHGRAPH_H
#define  HASHGRAPH_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "Graph.h"

using namespace std;

class HashGraph {
	private:
		const static int PRIME = 9854791;
		Graph* hTable[PRIME];
		//typedef map< int, Graph* > HashTable;
		//typedef HashTable::iterator it_HashTable;
		//HashTable hTable;
		int HashFunc(const Graph* g);
		unsigned int BKDRHash(const string s);
		bool Compare(const Graph* g1, const Graph* g2);
		int crashTime;
		int keyNum;
		int hitTime;
		int nodeNum;

	public:
		HashGraph();
		~HashGraph();
		Graph* insert(Graph *g);
		void Print() const;
		inline int TimesOfHit() const {return hitTime;}
		inline int TimesOfCrash() const {return crashTime;}
		inline int KeyNum() const {return keyNum;}
		inline int NodeNum() const {return nodeNum;}
};

#endif  /*HASHGRAPH_H*/
