#include "Graph.h"

Graph::Graph() {
	nNode = 0;
	nEffectiveEdge = 0;
	nEdge = 0;
	next = NULL;
	addFlag = true;
}

Graph::Graph(int n) {
	nNode = n;
	nEffectiveEdge = 0;
	nEdge = 0;
	next = NULL;
	addFlag = true;
}

Graph::Graph(const Graph *otherGraph) {
	int E = otherGraph->nEffectiveEdge;
	edgeTable = new Edge[E];
	int j = 0;
	for(int i = 0; i < E; ++i) {
		while(otherGraph->edgeTable[j].type < 0) ++j;
		edgeTable[i] = otherGraph->edgeTable[j++];
	}
	nNode = otherGraph->nNode;
	nEffectiveEdge = otherGraph->nEffectiveEdge;
	nEdge = nEffectiveEdge;
	next = NULL;
	addFlag = false;
}

void Graph::addEdge(const Edge* e) {
	if(addFlag) {
		Edge tmpEdge(e->node1, e->node2, e->sym, e->type);
		eTable.push_back(tmpEdge);
		++nEffectiveEdge;
		++nEdge;
	}
}

void Graph::shortNullor() {
	list<Edge>::iterator it = eTable.begin();
	while((it->type == -1 || 
		   it->type == NU || 
		   it->type == NO) && (it != eTable.end())) {
		if(it->type == -1) { ++it; continue; }
		int ns = it->node1;
		int nl = it->node2;
		if(ns == nl) {it->type = -1; ++it; continue;}
		if(ns > nl) { int tmp = ns; ns = nl; nl = tmp; }
		it->type = -1;
		--nNode;
		--nEffectiveEdge;
		if((nNode == 1) || (nEffectiveEdge == 0)) {
			cerr << "Something Wrong" << endl;
			exit(1);
		}
		list<Edge>::iterator ittmp = it;
		for(++ittmp; ittmp != eTable.end(); ++ittmp)
			if(ittmp->type != -1) {
				if(ittmp->node1 == nl) ittmp->node1 = ns;
				if(ittmp->node1 > nl) --ittmp->node1;
				if(ittmp->node2 == nl) ittmp->node2 = ns;
				if(ittmp->node2 > nl) --ittmp->node2;
				if(ittmp->node1 == ittmp->node2) {
					ittmp->type = -1;
					--nEffectiveEdge;
				}
			}
		++it;
	}
	genEdgeT();
}

void Graph::genEdgeT() {
	addFlag = false;
	edgeTable = new Edge[nEffectiveEdge];
	list<Edge>::const_iterator it = eTable.begin();
	int i = 0;
	while(it != eTable.end()) {
		while(it->type < 0) ++it;
		edgeTable[i++] = *it++;
	}
	nEdge = nEffectiveEdge;
	eTable.clear();
}

//0 for Can't Continue
//1 for tree
//2 for Can Continue
int Graph::Short(const Edge* e, bool &sign) {
	Edge* curEdge;
	Edge* it1 = edgeTable;
	Edge* it2 = NULL;
	if(nEdge) it2 = &edgeTable[1];
	if(!it2 || it2->sym != it1->sym) curEdge = it1; 
	else {
		if(e->type <= 3) curEdge = it1;
		else curEdge = it2;
	}
	if(curEdge->type == -1) return 0;
	if(curEdge->sym->name != e->sym->name ||
	   curEdge->type != e->type) return 0;
	int nl = curEdge->node1;
	int ns = curEdge->node2;
	if(ns == nl) return 0;
	else if(nl < ns) {
		int tmp = nl; nl = ns; ns = tmp;
		sign = !sign;
	}
	if(nl % 2) sign = !sign;
	--nNode;
	if(nNode == 1) {
		for(int i = 0; i < nEdge; ++i)
			edgeTable[i].type = -1;
		nEffectiveEdge = 0;
		return 1;
	}
	curEdge->type = -1;
	if(!(--nEffectiveEdge)) return 0;
	for(int i = 0; i < nEdge; ++i)
		if(edgeTable[i].type != -1) {
			if(edgeTable[i].node1 == nl) edgeTable[i].node1 = ns;
			if(edgeTable[i].node1 > nl) --edgeTable[i].node1;
			if(edgeTable[i].node2 == nl) edgeTable[i].node2 = ns;
			if(edgeTable[i].node2 > nl) --edgeTable[i].node2;
			if(edgeTable[i].node1 == edgeTable[i].node2) {
				edgeTable[i].type = -1;
				if(!(--nEffectiveEdge)) return 0;
			}
		}
	return 2;
}

//0 for Can't Continue
//1 for Can Continue
int Graph::Open(const Edge* e) {
	Edge* curEdge;
	Edge* it1 = edgeTable;
	Edge* it2 = NULL;
	if(nEdge) it2 = &edgeTable[1];
	if(!it2 || it2->sym != it1->sym) curEdge = it1;
	else {
		if(e->type <= 3) curEdge = it1;
		else curEdge = it2;
	}
	if(curEdge->sym->name != e->sym->name || curEdge->type != e->type) return 1;

	curEdge->type = -1;
	if(--nEffectiveEdge < nNode - 1) return 0;
	
	int n1 = curEdge->node1;
	int n2 = curEdge->node2;
	bool f1 = false, f2 = false;
	for(int i = 0; i < nEdge; ++i)
		if(edgeTable[i].type != -1) {
			if(n1 == edgeTable[i].node1 || n1 == edgeTable[i].node2) f1 = 1;
			if(n2 == edgeTable[i].node1 || n2 == edgeTable[i].node2) f2 = 1;
			if(f1 && f2) break;
		}

	if(f1 && f2) return 1;
	else return 0;
}

void Graph::printGraph() const {
	cout << "Graph Print..." << endl;
	cout << "Node Number:" << nNode << endl;
	cout << "Effective Edge:" << nEffectiveEdge << endl;
	cout << "Type\tNode1\tNode2\tSymName" << endl;
	for(int i = 0; i < nEdge; ++i)
		if(edgeTable[i].type >= 0) {
			switch (edgeTable[i].type) {
				case YZ: cout << "YZ\t"; break;
				case CC: cout << "CC\t"; break;
				case VC: cout << "VC\t"; break;
				case NU: cout << "NU\t"; break;
				case CS: cout << "CS\t"; break;
				case VS: cout << "VS\t"; break;
				case NO: cout << "NO\t"; break;
				default:
					break;
			}
			cout << edgeTable[i].node1 << '\t'
				 << edgeTable[i].node2 << '\t';
			cout << edgeTable[i].sym->name << endl;
		}
	cout << "Graph Print Complete." << endl;
}

Graph::~Graph() {
	delete [] edgeTable;
	eTable.clear();
}
