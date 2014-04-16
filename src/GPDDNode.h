#ifndef  GPDDNODE_H
#define  GPDDNODE_H

#include "GPDD_struct.h"
#include "Graph.h"


//May need More Information
//Should change GPDD::init()
struct GPDDNode {
	Symbol* sym;
	Graph* leftG;
	Graph* rightG;
	GPDDNode* inNode;
	GPDDNode* exNode;
	int numEFH;				//EFH number below this node
	bool inSign, exSign;	//true for 1, false for -1
	bool mark;				//For tranverse
	complex<double> value;
	long long termNum;

	complex<double> lValue;
	complex<double> rValue;
	
	//For Reduction
	GPDDNode* RNode;
	bool alterS;
	int NF;
	bool ZS;

	GPDDNode():
		sym(NULL), leftG(NULL), rightG(NULL), inNode(NULL), exNode(NULL),
		numEFH(0), inSign(true), exSign(true), mark(false), value(complex<double>(0,0)), termNum(0),
		alterS(false), RNode(NULL), NF(0), ZS(false){}
};

#endif  /*GPDDNODE_H*/
