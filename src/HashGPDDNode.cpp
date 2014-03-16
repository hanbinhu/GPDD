#include "HashGPDDNode.h"

HashGPDDNode::HashGPDDNode() {
	hTable.clear();
	crashTime = 0;
	hitTime = 0;
	nodeNum = 0;
}

HashGPDDNode::~HashGPDDNode() {
	it_HashTable it = hTable.begin();
	HashNode *tmp, *tmp0;
	while(it != hTable.end()) {
		tmp = (*it).second->next;
		while(tmp) {
			tmp0 = tmp;
			tmp = tmp->next;
			delete tmp0;
		}
		delete (*it).second;
		++it;
	}
	hTable.clear();
}

void HashGPDDNode::Print() const {
	cout << "HashGPDDNode Infomation:" << endl;
	cout << "Key Number: " << KeyNum() << " Node Number: " << NodeNum() << endl;
	cout << "Hit Time: " << TimesOfHit() << " Crash Time: " << TimesOfCrash() << endl;
	HashTable::const_iterator it = hTable.begin();
	HashNode* tmp;
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

int HashGPDDNode::HashFunc(const GPDDNode* node) {
	int hashNum = 0;
	if(node->leftG) 
		hashNum ^= int(node->leftG) << 8;
	if(node->rightG)
		hashNum ^= int(node->rightG);
	if(node->sym) {
		string s = node->sym->name;
		int hash_s = 0;
		for(int i = 0; i < s.length(); ++i) {
			hash_s ^= int(s[i]); hash_s = hash_s << 2;
		}
		hashNum ^= hash_s;
	}
	hashNum %= PRIME;
	return hashNum;
}

bool HashGPDDNode::Compare(const GPDDNode* node1, const GPDDNode* node2) {
	if(node1->sym == node2->sym && 
	   node1->leftG == node2->leftG && 
	   node1->rightG == node2->rightG) return true;
	return false;
}

GPDDNode* HashGPDDNode::insert(GPDDNode* node) {
	int key = HashFunc(node);
	it_HashTable it = hTable.find(key);
	if(it == hTable.end()) {
		HashNode* tmpNode = new HashNode;
		tmpNode->NodeInfo = node;
		tmpNode->next = NULL;
		hTable[key] = tmpNode;
		++nodeNum;
	} else {
		HashNode* tmpNode = (*it).second;
		HashNode* x = tmpNode;
		while(x) {
			if(Compare(node, x->NodeInfo)) {
				delete node;
				++hitTime;
				return x->NodeInfo;
			}
			x = x->next;
		}
		x = new HashNode;
		x->NodeInfo = node;
		x->next = tmpNode->next;
		tmpNode->next = x;
		++crashTime;
		++nodeNum;
	}
	return node;
}
