#include "HashReduction.h"

HashReduction::HashReduction() {
	hTable.clear();
	crashTime = 0;
	hitTime = 0;
	nodeNum = 0;
}

HashReduction::~HashReduction() {
	it_HashTable it = hTable.begin();
	HashR *tmp, *tmp0;
	while(it != hTable.end()) {
		tmp = it->second->next;
		while(tmp) {
			tmp0 = tmp;
			tmp = tmp->next;
			delete tmp0->NodeInfo;
			delete tmp0;
		}
		delete it->second->NodeInfo;
		delete it->second;
		++it;
	}
	hTable.clear();
}

void HashReduction::clean() {
	hTable.clear();
	crashTime = 0;
	hitTime = 0;
	nodeNum = 0;
}

void HashReduction::Print() const {
	cout << "HashReduction Infomation:" << endl;
	cout << "Key Number: " << KeyNum() << " Node Number: " << NodeNum() << endl;
	cout << "Hit Time: " << TimesOfHit() << " Crash Time: " << TimesOfCrash() << endl;
	HashTable::const_iterator it = hTable.begin();
	HashR* tmp;
	while(it != hTable.end()) {
		tmp = (*it).second;
		while(tmp) {
			cout << "Key Value: " << (*it).first << endl;
			cout << "Symbol: " << tmp->NodeInfo->sym->name << endl;
			tmp = tmp->next;
		}
		++it;
	}
}

int HashReduction::HashFunc(const GPDDNode* node) {
	int hashNum = 1;
	if(node->inNode) hashNum *= abs(int(node->inNode)) % PRIME;
	if(node->exNode) hashNum *= abs(int(node->exNode)) % PRIME;
	hashNum %= PRIME;
	return hashNum;
}

bool HashReduction::Compare(const GPDDNode* node1, const GPDDNode* node2, const GPDDNode* Zero) {
	if(node1->sym == node2->sym && 
	   node1->inNode == node2->inNode && 
	   node1->exNode == node2->exNode)
		if(node1->exNode == Zero ||
		   node1->inSign ^ node1->exSign == node2->inSign ^ node2->exSign) 
			return true;
	return false;
}

GPDDNode* HashReduction::insert(GPDDNode* node, bool &flag, const GPDDNode* Zero) {
	flag = false;
	int key = HashFunc(node);
	it_HashTable it = hTable.find(key);
	if(it == hTable.end()) {
		HashR* tmpNode = new HashR;
		tmpNode->NodeInfo = node;
		tmpNode->next = NULL;
		hTable[key] = tmpNode;
		++nodeNum;
	} else {
		HashR* tmpNode = (*it).second;
		HashR* x = tmpNode;
		while(x) {
			if(Compare(node, x->NodeInfo, Zero)) {
				++hitTime;
				flag = (node->inSign != x->NodeInfo->inSign);
				return x->NodeInfo;
			}
			x = x->next;
		}
		x = new HashR;
		x->NodeInfo = node;
		x->next = tmpNode->next;
		tmpNode->next = x;
		++crashTime;
		++nodeNum;
	}
	return node;
}
