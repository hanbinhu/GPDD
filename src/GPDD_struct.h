#ifndef  GPDD_STRUCT_H
#define  GPDD_STRUCT_H

#include <iostream>
#include <string>
#include <complex>

using namespace std;

struct Symbol;

struct Edge {
	int node1;
	int node2;
	Symbol *sym;
	int type;
	Edge() {}
	Edge(int n1, int n2, Symbol* s, int t):
		node1(n1), node2(n2), sym(s), type(t) {}
	Edge(const Edge &e):
		node1(e.node1), node2(e.node2), sym(e.sym), type(e.type) {}
};

struct Symbol {
	string name;
	Edge* e;
	Edge* pe;
	double v;
	complex<double> value;
	Symbol* lumpedNext;

	bool rd, so;
	bool rdEval, soEval;
	Symbol* shortSym;
	Symbol* shortSymShort;
	int delType;
	double errExtend;	

	~Symbol() {delete e; delete pe;}
};

#endif  /*GPDD_STRUCT_H*/
