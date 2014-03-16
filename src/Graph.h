#ifndef  GRAPH_H
#define  GRAPH_H

#include <iostream>
#include <list>
#include "Ckt.h"
#include "GPDD_struct.h"

using namespace std;

class Graph {
	private:
		bool addFlag;
		void genEdgeT();

	public:
		//construction
		Graph();
		Graph(int n);
		Graph(const Graph *otherGpaph);

		//function for GPDD Initialization
		void addEdge(const Edge* e);
		void shortNullor();

		//function for Expansion
		int Short(const Edge* e, bool &sign);
		int Open(const Edge* e);

		//print Function
		void printGraph() const;

		//Function for Hash
		list<Edge> eTable;
		Edge* edgeTable;
		int nNode;
		int nEffectiveEdge;
		int nEdge;
		Graph* next;

		//deconstruction
		~Graph();
};

#endif  /*GRAPH_H*/
