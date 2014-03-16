#include "HashGraph.h"

HashGraph::HashGraph() {
	for(int i = 0; i < PRIME; ++i)
		hTable[i] = NULL;
	crashTime = 0;
	hitTime = 0;
	nodeNum = 0;
	keyNum = 0;
}

void HashGraph::Print() const {
	cout << "HashGraph Infomation:" << endl;
	cout << "Key Number: " << KeyNum() << " Node Number: " << NodeNum() << endl;
	cout << "Hit Time: " << TimesOfHit() << " Crash Time: " << TimesOfCrash() << endl;
	for(int i = 0; i < PRIME; ++i) {
		if(hTable[i]) {
			cout << endl << "Key Value: " << i << endl;
			const Graph* x = hTable[i];
			while(x) {
				x->printGraph();
				cout << endl;
				x = x->next;
			}
		}
	}	
}

unsigned int HashGraph::BKDRHash(const string s) {
	unsigned int seed = 1313; // 31 131 1313 13131 131313 etc...
	unsigned int hash = 0;

	for(string::size_type i = 0; i != s.size(); ++i)
		hash = hash * seed + s[i];

	return (hash & 0x7FFFFFFF);
}

int HashGraph::HashFunc(const Graph* g) {
	if(!g->nEffectiveEdge) return 0;

	string hashString = "";
	ostringstream ss(hashString);
	ss << (g->nNode * g->nEffectiveEdge);
	hashString = ss.str();

	int k = 0;

	for(int i = 0; i < g->nEdge; ++i)
		if(g->edgeTable[i].type != -1) {
			hashString += g->edgeTable[i].sym->name;
			hashString += g->edgeTable[i].type;
			if(++k == 6) break;
		}
	int hashNum = BKDRHash(hashString);
	hashNum %= PRIME;
	return hashNum;
}

bool HashGraph::Compare(const Graph* g1, const Graph* g2) {
	int N1 = g1->nNode, N2 = g2->nNode;
	int E1 = g1->nEffectiveEdge, E2 = g2->nEffectiveEdge;
	if(N1 != N2 || E1 != E2) return false;
	int T1 = g1->nEdge, T2 = g2->nEdge;
	int i, j, k;

	for(i = 0, j = 0, k = 0; k < E1; ++i, ++j, ++k) {
		while(g1->edgeTable[i].type == -1) ++i;
		while(g2->edgeTable[j].type == -1) ++j;
		if(g1->edgeTable[i].sym != g2->edgeTable[j].sym ||
		   g1->edgeTable[i].node1 != g2->edgeTable[j].node1 ||
		   g1->edgeTable[i].node2 != g2->edgeTable[j].node2 ||
		   g1->edgeTable[i].type != g2->edgeTable[j].type) return false;
	}
	return true;
}

Graph* HashGraph::insert(Graph* g) {
	int key = HashFunc(g);
	if(!hTable[key]) {
		hTable[key] = g;
		++keyNum;
		++nodeNum;
	} else {
		Graph* tmp = hTable[key];
		Graph* tmp0 = tmp;
		while(tmp) {
			if(Compare(g, tmp)) {
				delete g;
				++hitTime;
				return tmp;
			}
			tmp = tmp->next;
		}
		g->next = tmp0->next;
		tmp0->next = g;
		++crashTime;
		++nodeNum;
	}
	return g;
}

HashGraph::~HashGraph() {
	for(int i = 0; i < PRIME; ++i) {
		Graph* tmp = hTable[i];
		Graph* tmp0 = tmp;
		while(tmp) {
			tmp0 = tmp->next;
			delete tmp;
			tmp = tmp0;
		}
	}
}
