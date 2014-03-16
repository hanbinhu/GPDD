#include <list>
#include <stdio.h>
#include "Ckt.h"
#include "SpParser.hpp"

extern FILE* yyin;

Ckt::Ckt() {
	src = false; out = false;
	numNode = 0; numEdge = 0;
	nodeList.clear();
	edgeList.clear();
	anaAC = new Analysis();
}

Ckt::Ckt(const char* fname) {
	src = false; out = false;
	numNode = 0; numEdge = 0;
	nodeList.clear();
	edgeList.clear();
	anaAC = new Analysis();
	cout << "Start Parsing..." << endl;
	yyin = fopen(fname, "r");
	if(!yyin) {
		cout << fname << "doesn't exist" << endl;
		exit(1);
	}
	yy::SpParser parser(this);
	parser.parse();
	anaAC->setOutFile(fname);
	fclose(yyin);
	cout << "Parsing Complete." << endl;

	cout << "NetList Simple Checking..." << endl;
	removeAllV();
	numberNode();
	if(!check_pair()) {
		cerr << "Something wrong with the netlist." << endl;
		exit(1);
	}

	cout << "Netlist Checking Complete." << endl;
}


void Ckt::parseRLC(const char* eleName, const char* nName1, const char*nName2, double v) {
	cktEdge* tmpEdge = findEdge(eleName);

	if(tmpEdge) {
		cerr << eleName << " already exist" << endl;
		exit(1);
	}

	tmpEdge = new cktEdge;
	tmpEdge->name = string(eleName);
	tmpEdge->node1 = insertNode(nName1);
	tmpEdge->node2 = insertNode(nName2);
	tmpEdge->pairEdge = NULL;
	tmpEdge->vname = "";
	tmpEdge->val = v;
	tmpEdge->type = YZ;
	tmpEdge->Vused = false;

	edgeList.push_back(tmpEdge);
	numEdge++;
}

void Ckt::parseVCXS(const char* eleName, const char* XSName1, const char* XSName2, const char* VCName1, const char* VCName2, double v) {
	if(eleName[0] == 'G') {
		bool f1 = !strcmp(XSName1, VCName1) && !strcmp(XSName2, VCName2);
		bool f2 = !strcmp(XSName1, VCName2) && !strcmp(XSName2, VCName1);
		if(f1 || f2) {
			if(f1 && f2) {
				cerr << "Wrong Connection of G" << endl;
				exit(1);
			}
			string s = string(eleName);
			s.append(";");
			s.insert(0, 1, 'R');
			if(f1) parseRLC(s.c_str(), XSName1, XSName2, v);
			else if(f2) parseRLC(s.c_str(), XSName1, XSName2, -v);
			return;
		}
	}

	cktEdge *tmpEdge1 = findEdge(eleName);

	if(tmpEdge1) {
		cerr << eleName << " already exist." << endl;
		exit(1);
	}

	cktEdge *tmpEdge2 = new cktEdge;
	tmpEdge1 = new cktEdge;

	//VC setting
	tmpEdge1->name = string(eleName);
	tmpEdge1->vname = string(eleName);
	tmpEdge1->node1 = insertNode(VCName1);
	tmpEdge1->node2 = insertNode(VCName2);
	tmpEdge1->val = v;
	tmpEdge1->Vused = false;
	tmpEdge1->pairEdge = tmpEdge2;

	//XS setting
	tmpEdge2->name = string(eleName);
	tmpEdge2->vname = string(eleName);
	tmpEdge2->node1 = insertNode(XSName1);
	tmpEdge2->node2 = insertNode(XSName2);
	tmpEdge2->val = v;
	tmpEdge2->Vused = false;
	tmpEdge2->pairEdge = tmpEdge1;

	//type setting
	switch (eleName[0]) {
		case 'E': 
		case 'G': tmpEdge1->type = VC; break;
		case 'N': tmpEdge1->type = NU; break;
		default:
			cerr << "No possible!" << endl;
			exit(1);
	}

	switch (eleName[0]) {
		case 'E': tmpEdge2->type = VS; break;
		case 'G': tmpEdge2->type = CS; break;
		case 'N': tmpEdge2->type = NO; break;
		default:
			cerr << "No possible!" << endl;
			exit(1);
	}

	edgeList.push_back(tmpEdge1);
	edgeList.push_back(tmpEdge2);
	numEdge += 2;
}

void Ckt::parseCCXS(const char* eleName, const char* XSName1, const char* XSName2, const char* VName, double v) {
	cktEdge *tmpEdge1 = findEdge(eleName);

	if(tmpEdge1) {
		cerr << eleName << " already Exist." << endl;
		exit(1);
	}

	tmpEdge1 = new cktEdge;
	tmpEdge1->name = string(eleName);
	tmpEdge1->node1 = insertNode(XSName1);
	tmpEdge1->node2 = insertNode(XSName2);
	tmpEdge1->val = v;
	tmpEdge1->Vused = false;
	tmpEdge1->vname = string(VName);
	if(eleName[0] == 'F') tmpEdge1->type = CS;
	else tmpEdge1->type = VS;

	cktEdge *tmpEdge2 = findCCEdge(VName);
	tmpEdge2->name = string(eleName);
	tmpEdge2->vname = string(VName);
	tmpEdge2->val = v;
	tmpEdge2->Vused = false;
	tmpEdge2->type = CC;
	tmpEdge2->pairEdge = tmpEdge1;
	tmpEdge1->pairEdge = tmpEdge2;
	
	edgeList.push_back(tmpEdge2);
	edgeList.push_back(tmpEdge1); 
	numEdge += 2;
}

void Ckt::parseVsrc(const char* eleName, const char* nName1, const char* nName2, double v) {
	if(findEdge(eleName)) {
		cerr << eleName << " already Exist." << endl;
		exit(1);
	}

	if(v == 0) {
		cktEdge* tmpEdge = new cktEdge;
		tmpEdge->name = string(eleName);
		tmpEdge->vname = string(eleName);
		tmpEdge->node1 = insertNode(nName1);
		tmpEdge->node2 = insertNode(nName2);
		tmpEdge->val = 0;
		tmpEdge->type = CC;
		tmpEdge->pairEdge = NULL;
		tmpEdge->Vused = false;
		findVName(tmpEdge);
		edgeList.push_back(tmpEdge);
	}else{
		if(src) {
			cerr << "Two Source exist." << endl;
		}

		//Voltage Setting
		cktEdge* tmpEdge = new cktEdge;
		tmpEdge->name = string(eleName);
		tmpEdge->node1 = insertNode(nName1);
		tmpEdge->node2 = insertNode(nName2);
		tmpEdge->type = VS;
		tmpEdge->Vused = false;
		tmpEdge->val = -1;

		//Setting for Outting
		if(out) {
			list<cktEdge*>::iterator it = edgeList.begin();
			tmpEdge->pairEdge = (*it);
			tmpEdge->vname = (*it)->name;
			(*it)->pairEdge = tmpEdge;
			if((*it)->type == VC) (*it)->name = "E" + (*it)->name;
			else (*it)->name = "H" + (*it)->name;
			tmpEdge->name = (*it)->name;
			if((*it)->type == VC) tmpEdge->vname = tmpEdge->name;
			else tmpEdge->vname = (*it)->vname;
			edgeList.insert(++it, tmpEdge);
		}else{
			tmpEdge->vname = string(eleName);
			tmpEdge->pairEdge = NULL;
			edgeList.push_front(tmpEdge);
		}
		
		src = true;
		numEdge++;
	}
}
void Ckt::parseIsrc(const char* eleName, const char* nName1, const char* nName2, double v) {
	if(src) {
		cerr << "Two Source exist." << endl;
		exit(1);
	}
	if(v == 0) {
		cerr << "Current source can't be zero." << endl;
		exit(1);
	}

	//Current Source Setting
	cktEdge* tmpEdge = new cktEdge;
	tmpEdge->name = string(eleName);
	tmpEdge->node1 = insertNode(nName1);
	tmpEdge->node2 = insertNode(nName2);
	tmpEdge->type = CS;
	tmpEdge->val = -1;
	tmpEdge->Vused = false;

	//Setting for Outting;
	if(out) {
		list<cktEdge*>::iterator it = edgeList.begin();
		tmpEdge->pairEdge = (*it);
		tmpEdge->vname = (*it)->name;
		(*it)->pairEdge = tmpEdge;
		if((*it)->type == VC) (*it)->name = "G" + (*it)->name;
		else (*it)->name = "F" + (*it)->name;
		tmpEdge->name = (*it)->name;
		if((*it)->type == VC) tmpEdge->vname = tmpEdge->name;
		else tmpEdge->vname = (*it)->vname;
		edgeList.insert(++it, tmpEdge);
	}else{
		tmpEdge->vname = string(eleName);
		tmpEdge->pairEdge = NULL;
		edgeList.push_front(tmpEdge);
	}

	src = true;
	numEdge++;
}

void Ckt::parseOutVC(const char* nName1, const char* nName2) {
	if(out) {
		cerr << "Two Output exists." << endl;
		exit(1);
	}

	cktEdge* tmpEdge = new cktEdge;
	tmpEdge->name = "Out;";
	tmpEdge->vname = "Out;";
	tmpEdge->node1 = insertNode(nName1);
	tmpEdge->node2 = insertNode(nName2);
	tmpEdge->type = VC;
	tmpEdge->val = -1;
	tmpEdge->Vused = false;

	if(src) {
		list<cktEdge*>::iterator it = edgeList.begin();
		tmpEdge->pairEdge = (*it);
		(*it)->pairEdge = tmpEdge;
		if((*it)->type == VS) tmpEdge->name = "E" + tmpEdge->name;
		else tmpEdge->name = "G" + tmpEdge->name;
		(*it)->name = tmpEdge->name;
		tmpEdge->vname = tmpEdge->name;
		(*it)->vname = tmpEdge->name;
	}else tmpEdge->pairEdge = NULL;

	edgeList.push_front(tmpEdge);
	out = true;
	numEdge++;
}

void Ckt::parseOutCC(const char* volName) {
	if(out) {
		cerr << "Two Output exists." << endl;
		exit(1);
	}

	cktEdge* tmpEdge = findCCEdge(volName);
	tmpEdge->name = "Out;";
	tmpEdge->vname = string(volName);
	tmpEdge->type = CC;
	tmpEdge->val = -1;
	tmpEdge->Vused = false;

	if(src) {
		list<cktEdge*>::iterator it = edgeList.begin();
		tmpEdge->pairEdge = (*it);
		tmpEdge->vname = volName;
		(*it)->pairEdge = tmpEdge;
		if((*it)->type == VS) tmpEdge->name = "H" + tmpEdge->name;
		else tmpEdge->name = "F" + tmpEdge->name;
		(*it)->name = tmpEdge->name;
		(*it)->vname = volName;
	}else tmpEdge->pairEdge = NULL;
	
	edgeList.push_front(tmpEdge); 
	out = true;
	numEdge++;
}

void Ckt::parseAC(int type, int step, double s, double e) {
	anaAC->ACInitial(type, step, s, e);
}

void Ckt::printCkt() const {
	cout << "Start to print Ckt..." << endl;
	cout << "All Node(" << numNode << "):" << endl;
	cout << "Index\tName" << endl;
	list<cktNode*>::const_iterator it1 = nodeList.begin();
	while(it1 != nodeList.end()) {
		cout << (*it1)->index << '\t' << (*it1)->name << endl;
		++it1;
	}

	cout << "All Edge(" << numEdge << "):" << endl;
	cout << "TYPE\tName\tiNode1\tiNode2\tValue\tVname\tpairEdge\tReduce\tSorO" << endl;
	list<cktEdge*>::const_iterator it2 = edgeList.begin();
	while(it2 != edgeList.end()) 
	{
		cout << edgeTypeTranslate((*it2)->type) << '\t' << (*it2)->name << '\t'
			 << nodeTypeTranslate((*it2)->node1) << '\t' << nodeTypeTranslate((*it2)->node2) << "\t"
			 << (*it2)->val << '\t';
		if((*it2)->vname == "") cout << "No V\t";
		else cout << (*it2)->vname << '\t';
		if((*it2)->pairEdge == NULL) cout << "No pair\t\t";
		else cout << edgeTypeTranslate((*it2)->pairEdge->type) << (*it2)->pairEdge->name << "\t\t";
		cout << endl;
		++it2;
	}
	anaAC->printACInf();
}

int Ckt::nodeTypeTranslate(cktNode* node) const {
	if(node == NULL) return -1;
	return node->index;
}

string Ckt::edgeTypeTranslate(int type) const {
	string tmp;
	switch (type) {
		case YZ: tmp = "YZ"; break;
		case CC: tmp = "CC"; break;
		case VC: tmp = "VC"; break;
		case NU: tmp = "NU"; break;
		case CS: tmp = "CS"; break;
		case VS: tmp = "VS"; break;
		case NO: tmp = "NO"; break;
		default: tmp = "Wrong";
	}
	return tmp;
}

cktEdge* Ckt::findEdge(const char *eName) {
	list<cktEdge*>::iterator it = edgeList.begin();
	while(it != edgeList.end()) {
		if(!strcasecmp(eName, (*it)->name.c_str()))
			return (*it);
		it++;
	}
	return NULL;
}

cktEdge* Ckt::findCCEdge(const char *eName) {
	list<cktEdge*>::iterator it = edgeList.begin();
	while(it != edgeList.end()) {
		if(!strcasecmp(eName, (*it)->name.c_str()) && !(*it)->Vused)
			break;
		it++;
	}
	cktEdge* tmpEdge = new cktEdge;
	tmpEdge->node1 = NULL;
	tmpEdge->node2 = NULL;
	if(it == edgeList.end()) return tmpEdge;
	else {
		tmpEdge->node1 = (*it)->node1;
		tmpEdge->node2 = (*it)->node2;

		(*it)->Vused = true;

		return tmpEdge;
	}
}

cktEdge* Ckt::findVName(cktEdge* VEdge) {
	list<cktEdge*>::iterator it = edgeList.begin();
	while(it != edgeList.end()) {
		if(!strcasecmp(VEdge->name.c_str(), (*it)->vname.c_str())) break;
		it++;
	}
	if(it == edgeList.end()) return NULL;
	(*it)->node1 = VEdge->node1;
	(*it)->node2 = VEdge->node2;
	VEdge->Vused = true;
	return (*it);
}

cktNode* Ckt::insertNode(const char *nName) {
	list<cktNode*>::iterator it = nodeList.begin();
	while(it != nodeList.end()) {
		if(!strcasecmp(nName, (*it)->name.c_str())) 
			return (*it);
		it++;
	}

	cktNode* tmpNode = new cktNode;
	tmpNode->name = string(nName);

	if(!strcmp(nName,"0")) nodeList.push_front(tmpNode);
	else nodeList.push_back(tmpNode);
	numNode++;

	return tmpNode;
}

void Ckt::removeAllV() {
	list<cktEdge*>::iterator it = edgeList.begin();
	while(it != edgeList.end()) {
		if((*it)->name[0] == 'V' && (*it)->Vused) 
			it = edgeList.erase(it);
		else it++;
	}
}

void Ckt::numberNode() {
	int p = 0;
	list<cktNode*>::iterator it = nodeList.begin();
	while(it != nodeList.end()) {
		(*it)->index = p++;
		it++;
	}
}

bool Ckt::check_pair() {
	list<cktEdge*>::iterator it = edgeList.begin();
	while(it != edgeList.end()) {
		if((*it)->type > 0)
			if(!(*it)->node1 || !(*it)->node2 || !(*it)->pairEdge) 
				return false;
		it++;
	}
	return true;
}

Ckt::~Ckt() {
	list<cktNode*>::iterator it1 = nodeList.begin();
	while(it1 != nodeList.end()) delete (*it1++);
	nodeList.clear();
	list<cktEdge*>::iterator it2 = edgeList.begin();
	while(it2 != edgeList.end()) delete (*it2++);
	edgeList.clear();
	delete anaAC;
}
