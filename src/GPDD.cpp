#include "GPDD.h"

GPDD::GPDD(const Ckt* myCkt) {
	cout << "Start GPDD Construction from Circuit" << endl;

	struct timeval start, finish;
	gettimeofday(&start, NULL);

	symbolList.clear();
	nullorList.clear();

	tTime = 0;

	if(!myCkt->pairInOut()) {
		cerr << "No Input and Output for GPDD" << endl;
		exit(1);
	}

	numNode = myCkt->numN();
	numEdge = myCkt->numE();
	
	reduceTime = 0;

	inited = false;
	expanded = false;
	caled = false;
	termCounted = false;
	int n = 0;

	const list<cktEdge*>& eList = myCkt->eList();
	list<cktEdge*>::const_iterator it = eList.begin();
	while(it != eList.end()) {
		switch ((*it)->type) {
			case NU: 
			case VC:
			case CC: {
						 Symbol* tmpSym = new Symbol;
						 Edge* e1 = new Edge;
						 Edge* e2 = new Edge;
						 tmpSym->name = (*it)->name;
						 tmpSym->v = (*it)->val;
						 tmpSym->value = complex<double>(tmpSym->v, 0);
						 tmpSym->e = e1; tmpSym->pe = e2;
						 tmpSym->lumpedNext = NULL;
						 tmpSym->rd = false; tmpSym->so = false;
						 tmpSym->rdEval = false; tmpSym->soEval = false;
						 tmpSym->shortSym = NULL; tmpSym->shortSymShort = NULL;

						 e1->node1 = (*it)->node1->index;
						 e1->node2 = (*it)->node2->index;
						 e1->sym = tmpSym; e1->type = (*it)->type;
						 ++it;
						 e2->node1 = (*it)->node1->index;
						 e2->node2 = (*it)->node2->index;
						 e2->sym = tmpSym; e2->type = (*it)->type;

						 if((*it)->type == NO) nullorList.push_back(tmpSym);
						 else {
							 symbolList.push_back(tmpSym);
							 if(tmpSym->name[0] != 'G') ++maxEFH;
						 }
					 } break;
			case YZ: {
						 Symbol* tmpSym = new Symbol;
						 Edge* e = new Edge;
						 tmpSym->name = (*it)->name;
						 tmpSym->v = (*it)->val;
						 tmpSym->value = complex<double>(tmpSym->v, 0);
						 tmpSym->e = e; tmpSym->pe = NULL;
						 
						 tmpSym->rd = false; tmpSym->so = false;
						 tmpSym->rdEval = false; tmpSym->soEval = false;
						 tmpSym->shortSym = NULL; tmpSym->shortSymShort = NULL;

						 e->node1 = (*it)->node1->index;
						 e->node2 = (*it)->node2->index;
						 e->sym = tmpSym; e->type = (*it)->type;

						 list<Symbol*>::iterator ittmp = symbolList.begin();
						 while(ittmp != symbolList.end()) {
							 if((*ittmp)->e->type == YZ)
								 if((((*ittmp)->e->node1 == e->node1) && 
									 ((*ittmp)->e->node2 == e->node2)) ||
									(((*ittmp)->e->node1 == e->node2) && 
									 ((*ittmp)->e->node2 == e->node1))) {
									 tmpSym->lumpedNext = (*ittmp)->lumpedNext;
									 (*ittmp)->lumpedNext = tmpSym;
									 break;
								 }
							 ++ittmp;
						 }
						 if(ittmp == symbolList.end()) {
							 tmpSym->lumpedNext = NULL;
							 symbolList.push_back(tmpSym);
						 }
					 } break;
			default:
				break;
		}
		++it;
	}
	anaAC = myCkt->anaCkt();

	gettimeofday(&finish, NULL);
	cirTime = TIME_UNIT * (finish.tv_sec - start.tv_sec) + finish.tv_usec - start.tv_usec; 
	tTime += cirTime;

	cout << "GPDD Construction Complete." << endl;
}

void GPDD::init() {
	//GPDDRoot Initialization
	cout << "GPDD Initialzation Start..." << endl;
	cout << "GPDDRoot Initialization..." << endl;

	if(inited) return;

	struct timeval start, finish;
	gettimeofday(&start, NULL);

	GPDDRoot = new GPDDNode;
	list<Symbol*>::iterator it = symbolList.begin();
	GPDDRoot->sym = *it; 
	GPDDRoot->numEFH = maxEFH;
	if(GPDDRoot->sym->name[0] != 'G') --GPDDRoot->numEFH;

	//Initializtion for GPDDRoot's Graph
	Graph* lGraph = new Graph(numNode);
	Graph* rGraph = new Graph(numNode);
	
	for(it = nullorList.begin(); it != nullorList.end(); ++it) {
		lGraph->addEdge((*it)->pe);
		rGraph->addEdge((*it)->e);
	}
	
	for(it = symbolList.begin(); it != symbolList.end(); ++it) {
		if((*it)->e->type == YZ) {
			lGraph->addEdge((*it)->e);
			rGraph->addEdge((*it)->e);
		}else{
			if((*it)->e->type != VC)
				lGraph->addEdge((*it)->e);
			lGraph->addEdge((*it)->pe);
			rGraph->addEdge((*it)->e);
			if((*it)->pe->type != CS)
				rGraph->addEdge((*it)->pe);
		}
	}

	lGraph->shortNullor();
	rGraph->shortNullor();

	lGraph = graphHash.insert(lGraph);
	rGraph = graphHash.insert(rGraph);
	//Need Hash Consideration
	GPDDRoot->leftG = lGraph;
	GPDDRoot->rightG = rGraph;

	GPDDRoot = GPDDNodeHash.insert(GPDDRoot);

	gettimeofday(&finish, NULL);
	initTime = TIME_UNIT * (finish.tv_sec - start.tv_sec) + finish.tv_usec - start.tv_usec; 

	//GPDDRoot may need Hash Consideration

	//GPDDZero & GPDDOne Initializtion
	cout << "GPDDZero & GPDDOne Initialization" << endl;

	gettimeofday(&start, NULL);

	GPDDZero = new GPDDNode; 
	GPDDZero->mark = true; GPDDZero->value = complex<double>(0, 0);
	GPDDZero->RNode = GPDDZero;  

	GPDDOne = new GPDDNode; 
	GPDDOne->mark = true; GPDDOne->value = complex<double>(1, 0);
	GPDDOne->RNode = GPDDOne; GPDDOne->termNum = 1; 

	inited = true;

	gettimeofday(&finish, NULL);
	initTime += TIME_UNIT * (finish.tv_sec - start.tv_sec) + finish.tv_usec - start.tv_usec; 
	tTime += initTime;

	cout << "GPDD Initialization Complete." << endl;
}

void GPDD::printConnection(const GPDDNode* curNode) const {
	cout << curNode->sym->name << endl;
	if(curNode->inNode == NULL) {
		cout << "N N" << endl;
	} else if (curNode->exNode == NULL) {
		if(curNode->inSign) cout << "+";
		else cout << "-";
		if(curNode->inNode == GPDDOne) cout << "1";
		else if(curNode->inNode == GPDDZero) cout << "0";
		else cout << curNode->inNode->sym->name;
		cout << " N" << endl;
	} else {
		if(curNode->inSign) cout << "+";
		else cout << "-";
		if(curNode->inNode == GPDDOne) cout << "1";
		else if(curNode->inNode == GPDDZero) cout << "0";
		else cout << curNode->inNode->sym->name;
		if(curNode->exSign) cout << " +";
		else cout << " -";
		if(curNode->exNode == GPDDOne) cout << "1";
		else if(curNode->exNode == GPDDZero) cout << "0";
		else cout << curNode->exNode->sym->name;
		cout << endl;
	}
}

void GPDD::printGPDD() const {
	printSymbol();

	stack<GPDDNode*> s;
	bool tmpMark = GPDDRoot->mark;
	s.push(GPDDRoot);
	while(!s.empty()) {
		GPDDNode* cur = s.top();
		cout << cur << endl;
		printConnection(cur);
		cout << cur->inNode << " " << cur->exNode << endl;
		cin.get();
		if(tmpMark == cur->inNode->mark) s.push(cur->inNode);
		else if(tmpMark == cur->exNode->mark) s.push(cur->exNode);
		else {
			cur->mark = !tmpMark;
			s.pop();
		}
	}

	GPDDRoot->mark = !tmpMark;
	GPDDZero->mark = tmpMark;
	GPDDOne->mark = tmpMark;
}

void GPDD::stepExpandPrint(const GPDDNode* curNode) const {
	printConnection(curNode);
	cout << "Hash Information:" << endl;
	cout << "Graph Number:\t" << graphHash.NodeNum()
		 << "\tGraph Hash:\t" << graphHash.TimesOfHit() << endl;
	cout << "Node Number:\t" << GPDDNodeHash.NodeNum()
		 << "\tNode Hash:\t" << GPDDNodeHash.TimesOfHit() << endl;
	curNode->leftG->printGraph();
	curNode->rightG->printGraph(); 
}

void GPDD::expand(bool step) {
	cout << "GPDD Expansion Start..." << endl;

	if(!inited) {
		cerr << "GPDD has not been initialized." << endl;
		exit(1);
	}
	if(expanded) return;

	struct timeval start, finish;
	gettimeofday(&start, NULL);

	stepExpand = step;

	stack<GPDDNode*> s;
	list<Symbol*>::iterator it = symbolList.begin();
	s.push(GPDDRoot);
	while(!s.empty()) {
		GPDDNode* curNode = s.top();

		if(stepExpand) { 
			stepExpandPrint(curNode);
			cin.get();
		}

		if(curNode->inNode == NULL) {
			pair<GPDDNode*, bool> inInfo;
			inInfo = Include(curNode);
			if(inInfo.first != GPDDOne && inInfo.first != GPDDZero) {
				++it;
				inInfo.first->sym = *it;
				char c = inInfo.first->sym->name[0];
				if(c == 'E' || c == 'F' || c == 'H')
					--inInfo.first->numEFH;
				inInfo.first = GPDDNodeHash.insert(inInfo.first);
				s.push(inInfo.first);
			}
			curNode->inNode = inInfo.first;
			inInfo.first->NF++;
			curNode->inSign = inInfo.second;
		} else if (curNode->exNode == NULL) {
			pair<GPDDNode*, bool> exInfo;
			exInfo = Exclude(curNode);
			if(exInfo.first != GPDDOne && exInfo.first != GPDDZero) {
				++it;
				exInfo.first->sym = *it;
				char c = exInfo.first->sym->name[0];
				if(c == 'E' || c == 'F' || c == 'H')
					--exInfo.first->numEFH;
				exInfo.first = GPDDNodeHash.insert(exInfo.first);
				s.push(exInfo.first);
			}
			curNode->exNode = exInfo.first;
			exInfo.first->NF++;
			curNode->exSign = exInfo.second;
		} else {
			s.pop();
			if(!s.empty()) --it;
		}
	}

	expanded = true;

	gettimeofday(&finish, NULL);
	expandTime = TIME_UNIT * (finish.tv_sec - start.tv_sec) + finish.tv_usec - start.tv_usec; 
	tTime += expandTime;
	
	graphNode[0] = graphHash.NodeNum();
	graphNode[1] = graphHash.TimesOfHit();
	graphNode[2] = graphHash.TimesOfCrash();
	expandNode[0] = GPDDNodeHash.NodeNum();
	expandNode[1] = GPDDNodeHash.TimesOfHit();
	expandNode[2] = GPDDNodeHash.TimesOfCrash();

	cout << "GPDD Expansion Complete." << endl;
}

pair<GPDDNode*, bool> GPDD::Include(const GPDDNode* fNode) {
	bool sign = true, signl = true, signr = true;
	int flag, flagl, flagr;
	const Symbol* curSym = fNode->sym;
	Graph* lGraph = new Graph(fNode->leftG);
	Graph* rGraph = new Graph(fNode->rightG);
	switch (curSym->name[0]) {
		case 'R':
		case 'L':
		case 'C': { flagl = lGraph->Short(curSym->e, signl);
					flagr = rGraph->Short(curSym->e, signr);
					if(flagl == 1 && flagr == 1) flag = 1;
					else if(flagl == 2 && flagr == 2) flag = 2;
					else flag = 0;
					if(flag) sign = !(signl ^ signr);
				  }	break;
		case 'E': { flagl = lGraph->Short(curSym->pe, signl);
					flagr = rGraph->Short(curSym->e, signr);
					if(flagl == 1 && flagr == 1) flag = 1;
					else if(flagl == 2 && flagr == 2){
					    flagr = rGraph->Open(curSym->pe);
						if(flagr) flag = 2;
						else flag = 0;
					} else flag = 0;
					if(flag) sign = !(signl ^ signr) ^ true;
				  }	break;
		case 'F': { flagl = lGraph->Short(curSym->pe, signl);
					flagr = rGraph->Short(curSym->e, signr);
					if(flagl == 1 && flagr == 1) flag = 1;
					else if(flagl == 2 && flagr == 2) {
					    flagl = lGraph->Open(curSym->e);
						if(flagl) flag = 2;
						else flag = 0;
					} else flag = 0;
					if(flag) sign = !(signl ^ signr);
				  }	break;
		case 'G': { flagl = lGraph->Short(curSym->pe, signl);
					flagr = rGraph->Short(curSym->e, signr);
					if(flagl == 1 && flagr == 1) flag = 1;
					else if(flagl == 2 && flagr == 2) flag = 2;
					else flag = 0;
					if(flag) sign = !(signl ^ signr);
				  }	break;
		case 'H': {	flagl = lGraph->Short(curSym->pe, signl);
					flagr = rGraph->Short(curSym->e, signr);
					if(flagl == 1 && flagr == 1) flag = 1;
					else if(flagl == 2 && flagr == 2) {
						flagl = lGraph->Open(curSym->e);
						flagr = rGraph->Open(curSym->pe);
						if(flagl == 1 && flagr == 1) flag = 2;
						else flag = 0;
					} else flag = 0;
					if(flag) sign = !(signl ^ signr) ^ true;
				  }	break;
		default: break;
	}
	switch (flag) {
		case 0: 
			delete lGraph; delete rGraph;
			return make_pair(GPDDZero, true); 
			break;
		case 1:	
			delete lGraph; delete rGraph;
			if(!fNode->numEFH) return make_pair(GPDDOne, sign); 
			else return make_pair(GPDDZero, true);
			break;
		case 2: { lGraph = graphHash.insert(lGraph);
				  rGraph = graphHash.insert(rGraph);
				  GPDDNode *newNode = new GPDDNode;
				  newNode->leftG = lGraph; newNode->rightG = rGraph;
				  newNode->numEFH = fNode->numEFH;
				  return make_pair(newNode, sign);
				} break;
		default: break;
	}
}

pair<GPDDNode*, bool> GPDD::Exclude(const GPDDNode* fNode) {
	bool sign = true, signl = true, signr = true;
	int flag, flagl, flagr;
	const Symbol* curSym = fNode->sym;
	Graph* lGraph = new Graph(fNode->leftG);
	Graph* rGraph = new Graph(fNode->rightG);
	switch (curSym->name[0]) {
		case 'R':
		case 'L':
		case 'C': { flagl = lGraph->Open(curSym->e);
					flagr = rGraph->Open(curSym->e);
					if(flagl == 1 && flagr == 1) flag = 2;
					else flag = 0;
					sign = true;
				  }	break;
		case 'E': { flagl = lGraph->Short(curSym->pe, signl);
					flagr = rGraph->Short(curSym->pe, signr);
					if(flagl == 1 && flagr == 1) flag = 1;
					else if(flagl == 2 && flagr == 2) {
						flagr = rGraph->Open(curSym->e);
						if(flagr) flag = 2;
						else flag = 0;
					} else flag = 0;
					if(flag) sign = !(signl ^ signr);
				  }	break;
		case 'F': { flagl = lGraph->Short(curSym->e, signl);
					flagr = rGraph->Short(curSym->e, signr);
					if(flagl == 1 && flagr == 1) flag = 1;
					else if(flagl == 2 && flagr == 2) {
						flagl = lGraph->Open(curSym->pe);
						if(flagl) flag = 2;
						else flag = 0;
					} else flag = 0;
					if(flag) sign = !(signl ^ signr);
				  }	break;
		case 'G': { flagl = lGraph->Open(curSym->pe);
					flagr = rGraph->Open(curSym->e);
					if(flagl == 1 && flagr == 1) flag = 2;
					else flag = 0;
					sign = true;
				  }	break;
		case 'H': { flagl = lGraph->Short(curSym->e, signl);
					flagr = rGraph->Short(curSym->e, signr);
					if(flagl == 0 || flagr == 0) flag = 0;
					else {
						flagl = lGraph->Short(curSym->pe, signl);
						flagr = rGraph->Short(curSym->pe, signr);
						if(flagl == 1 && flagr == 1) flag = 1;
						else if(flagl == 2 && flagr == 2) flag = 2;
						else flag = 0;
					}
					sign = !(signl ^ signr);
				  }	break;
		default: break;
	}
	switch (flag) {
		case 0: 
			delete lGraph; delete rGraph;
			return make_pair(GPDDZero, true); 
			break;
		case 1:	
			delete lGraph; delete rGraph;
			if(!fNode->numEFH) return make_pair(GPDDOne, sign); 
			else return make_pair(GPDDZero, true);
			break;
		case 2: { lGraph = graphHash.insert(lGraph);
				  rGraph = graphHash.insert(rGraph);
				  GPDDNode *newNode = new GPDDNode;
				  newNode->leftG = lGraph; newNode->rightG = rGraph;
				  newNode->numEFH = fNode->numEFH;
				  return make_pair(newNode, sign);
				} break;
		default: break;
	}
}

void GPDD::reduction() {
	cout << "GPDD Reduction Start..." << endl;

	if(!expanded) {
		cerr << "GPDD has not been expanded." << endl;
		exit(1);
	}

	struct timeval start, finish;
	gettimeofday(&start, NULL);
	ReductionHash.clean();

	//For Reduction
	stack<GPDDNode*> s;
	s.push(GPDDRoot);
	while(!s.empty()) {
		GPDDNode* cur = s.top();
		GPDDNode* in = cur->inNode;
		GPDDNode* ex = cur->exNode;
		while(in->ZS) {
			cur->inNode = in->exNode;
			cur->inSign = !(cur->inSign ^ in->exSign);
			++cur->inNode->NF;
			if(!--in->NF) {
				--in->inNode->NF;
				--in->exNode->NF;
				delete in;
			}
			in = cur->inNode;
		}
		if(in == GPDDZero) {
			cur->ZS = true;
			s.pop();
			continue;
		}
		if(in->RNode) {
			if(in != in->RNode) {
				cur->inSign ^= in->alterS;
				cur->inNode = in->RNode;
				++in->RNode->NF;
				if(!--in->NF) {
					--in->inNode->NF;
					--in->exNode->NF;
					delete in;
				}
			}
		} else {
			s.push(in);
			continue;
		}
		while(ex->ZS) {
			cur->exNode = ex->exNode;
			cur->exSign = !(cur->exSign ^ ex->exSign);
			++cur->exNode->NF;
			if(!--ex->NF) {
				--ex->inNode->NF;
				--ex->exNode->NF;
				delete ex;
			}
			ex = cur->exNode;
		}
		if(ex->RNode) {
			if(ex != ex->RNode) {
				if(GPDDZero != cur->exNode) cur->exSign ^= ex->alterS;
				cur->exNode = ex->RNode;
				++ex->RNode->NF;
				if(!--ex->NF) {
					--ex->inNode->NF;
					--ex->exNode->NF;
					delete ex;
				}
			}
		} else {
			s.push(ex);
			continue;
		}
		cur->RNode = ReductionHash.insert(cur, cur->alterS, GPDDZero);
		s.pop();
	}

	gettimeofday(&finish, NULL);
	reduceTime = TIME_UNIT * (finish.tv_sec - start.tv_sec) + finish.tv_usec - start.tv_usec; 
	tTime += reduceTime;

	reduceNode[0] = ReductionHash.NodeNum();
	reduceNode[1] = ReductionHash.TimesOfHit();
	reduceNode[2] = ReductionHash.TimesOfCrash();

	cout << "GPDD Reduction Complete." << endl;
}

void GPDD::Calculation() {
	if(!expanded) {
		cerr << "GPDD has not been expanded." << endl;
		exit(1);
	}

	string outFile = anaAC->outFile();

	ofstream out;
	out.open(outFile.c_str());
	if(!out) {
		cerr << "Cannot Open OutFile." << endl;
		exit(1);
	}
	if(!anaAC->isSet()) {
		cout << "AC Analysis haven't been setted." << endl;
		return;
	}
	out.setf(ios::scientific | ios::left);
	out.precision(8);
	cout << "Frequency Calculation Start..." << endl;

	struct timeval start, finish;
	long CalTime = 0;

	complex<double> res;
	const vector<double>& allFreq = anaAC->sweep();
	vector<double>::const_iterator it = allFreq.begin();
	while(it != allFreq.end()) {
		gettimeofday(&start, NULL);
		double freq = *it;
		updateSymbol(freq);
		res = Evaluate();
		double mag = abs(res);
		double phase = arg(res) / PI * 180;
		magList.push_back(20 * log(mag) / log(10)); phaseList.push_back(phase);
		gettimeofday(&finish, NULL);
		CalTime += TIME_UNIT * (finish.tv_sec - start.tv_sec) + finish.tv_usec - start.tv_usec;
		out << setw(20) << freq << setw(20) << mag << setw(20) << phase << endl;
		++it;
	}
	out.close();

	caled = true;
	wholeCalTime = CalTime;
	onceAvgCalTime = CalTime / allFreq.size();
	tTime += CalTime;

	cout << "All Result are written into " << outFile <<"." << endl;
}

void GPDD::updateSymbol(const double freq) {
	list<Symbol*>::iterator it = symbolList.begin();
	while(it != symbolList.end()) {
		if((*it)->e->type == YZ) {
			complex<double> tmp(0, 0);
			Symbol* p = *it;
			while(p) {
				switch (p->name[0]) {
					case 'R': p->value = complex<double>(p->v, 0); break;
					case 'C': p->value = complex<double>(0, 2 * PI * freq * p->v);	break;
					case 'L': p->value = complex<double>(0, p->v / 2 / PI / freq);	break;
					default: break;
				}
				tmp += p->value;
				p = p->lumpedNext;
			}
			(*it)->value = tmp;
		} else (*it)->value = complex<double>((*it)->v, 0);
		++it;
	}
}

void GPDD::TermCount() {
	if(termCounted) return;

	nTermNum = 1;
	nTermDen = 1;

	stack<GPDDNode*> s;
	bool tmpMark = GPDDRoot->mark;
	if(GPDDRoot->exNode != GPDDOne && GPDDRoot->exNode != GPDDZero) 
		s.push(GPDDRoot->exNode);
	if(GPDDRoot->inNode != GPDDOne && GPDDRoot->inNode != GPDDZero) 
		s.push(GPDDRoot->inNode);
	while(!s.empty()) {
		GPDDNode* curNode = s.top();
		if(curNode->inNode->mark == tmpMark) s.push(curNode->inNode);
		else if(curNode->exNode->mark == tmpMark) s.push(curNode->exNode);
		else {
			curNode->mark = !tmpMark;
			curNode->termNum = curNode->inNode->termNum + curNode->exNode->termNum;
			s.pop();
		}
	}
	GPDDRoot->mark = !tmpMark;
	GPDDZero->mark = tmpMark;
	GPDDOne->mark = tmpMark;
	
	if(GPDDRoot->inNode != GPDDOne && GPDDRoot->inNode != GPDDZero) 
		nTermNum = GPDDRoot->inNode->termNum;
	if(GPDDRoot->exNode != GPDDOne && GPDDRoot->exNode != GPDDZero)
		nTermDen = GPDDRoot->exNode->termNum;

	termCounted = true;
}

complex<double> GPDD::Evaluate() const {
	stack<GPDDNode*> s;
	bool tmpMark = GPDDRoot->mark;
	if(GPDDRoot->exNode != GPDDOne && GPDDRoot->exNode != GPDDZero) 
		s.push(GPDDRoot->exNode);
	if(GPDDRoot->inNode != GPDDOne && GPDDRoot->inNode != GPDDZero) 
		s.push(GPDDRoot->inNode);
	while(!s.empty()) {
		GPDDNode* curNode = s.top();
		if(curNode->inNode->mark == tmpMark) s.push(curNode->inNode);
		else if(curNode->exNode->mark == tmpMark) s.push(curNode->exNode);
		else {
			curNode->mark = !tmpMark;
			curNode->value = curNode->sym->value * curNode->inNode->value;
			if(!curNode->inSign) curNode->value *= -1;
			if(curNode->exSign) curNode->value += curNode->exNode->value;
			else curNode->value -= curNode->exNode->value;
			s.pop();
		}
	}
	GPDDRoot->mark = !tmpMark;
	GPDDZero->mark = tmpMark;
	GPDDOne->mark = tmpMark;
	complex<double> final = -1;
	if(GPDDRoot->inSign^GPDDRoot->exSign) final *= -1;
	final *= GPDDRoot->inNode->value / GPDDRoot->exNode->value;
	return final;
}

void GPDD::delPreProcess() {
	list<Symbol*>::iterator it = symbolList.begin();
	while(it != symbolList.end()) {
		if((*it)->e->type == YZ) {
			bool f = false;
			Symbol* p = *it;
			while(p) {
				if(p->rd && !p->so) break;
				if(!p->rd) f = true;
				p = p->lumpedNext;
			}
			if(p) {(*it)->rdEval = true; (*it)->soEval = false;} 
			else if (f) {(*it)->rdEval = false; (*it)->soEval = false;}
			else {(*it)->rdEval = true; (*it)->soEval = true;}
		} else {
			(*it)->rdEval = (*it)->rd;
			(*it)->soEval = (*it)->so;
		}
		++it;
	}
					
	list<Symbol*>::reverse_iterator itr = symbolList.rbegin();
	Symbol* tmp = NULL;
	while(itr != symbolList.rend()) {
		(*itr)->shortSymShort = tmp;
		if((*itr)->rdEval && !(*itr)->soEval) tmp = *itr;
		(*itr)->shortSym = tmp;
		itr++;
	}
}

void GPDD::delUpdateSymbol(const double freq) {
	list<Symbol*>::iterator it = symbolList.begin();
	while(it != symbolList.end()) {
		if((*it)->rdEval) {
			(*it)->value = complex<double>(0, 0);
			++it;
			continue;
		}
		if((*it)->e->type == YZ) {
			complex<double> tmp(0, 0);
			Symbol* p = *it;
			while(p) {
				if(p->rd) {p = p->lumpedNext; continue;}
				switch (p->name[0]) {
					case 'R': p->value = complex<double>(p->v, 0); break;
					case 'C': p->value = complex<double>(0, 2 * PI * freq * p->v);	break;
					case 'L': p->value = complex<double>(0, p->v / 2 / PI / freq);	break;
					default: break;
				}
				tmp += p->value;
				p = p->lumpedNext;
			}
			(*it)->value = tmp;
		} else (*it)->value = complex<double>((*it)->v, 0);
		++it;
	}
}

complex<double> GPDD::EvaluatePC(const GPDDNode* p, const GPDDNode* c) const {
	Symbol* ps = p->sym;
	Symbol* psS = ps->shortSym;

	if(c == GPDDOne || c == GPDDZero) {
		if(!ps->shortSymShort)
			return c->value;
		else return complex<double>(0, 0);
	}

	Symbol* cs = c->sym;
	Symbol* csS = cs->shortSym;

	complex<double> tmp;
	if((psS != csS && ps != psS) ||
	   (ps == psS && ps->shortSymShort != csS)) return complex<double>(0, 0);
	else if(cs == csS) {								
		tmp = c->lValue;
		if(!c->inSign) tmp *= -1;
		return tmp;
	} else {
		tmp = cs->value * c->lValue;
		if(!c->inSign) tmp *= -1;
		if(c->exSign) tmp += c->rValue;
		else tmp -= c->rValue;
		return tmp;
	}
}

complex<double> GPDD::delEvaluate() const {
	stack<GPDDNode*> s;
	s.push(GPDDRoot);
	
	bool tmpMark = GPDDRoot->mark;
	GPDDZero->mark = !tmpMark;
	GPDDOne->mark = !tmpMark;
	
	while(!s.empty()) {
		GPDDNode* curNode = s.top();
		GPDDNode* lNode = curNode->inNode;
		GPDDNode* rNode = curNode->exNode;
		if(lNode->mark == tmpMark) s.push(lNode);
		else if(rNode->mark == tmpMark) s.push(rNode);
		else {
			curNode->mark = !tmpMark;
			curNode->lValue = EvaluatePC(curNode, lNode);
			curNode->rValue = EvaluatePC(curNode, rNode);
			s.pop();
		}
	}
	
	GPDDRoot->mark = !tmpMark;
	GPDDZero->mark = tmpMark;
	GPDDOne->mark = tmpMark;
	complex<double> final = -1;
	if(GPDDRoot->inSign ^ GPDDRoot->exSign) final *= -1;
	final *= GPDDRoot->lValue / GPDDRoot->rValue;
	return final;
}

pair<double, double> GPDD::delCalculation(bool fout, const string& filename) {
	if(!expanded) {
		cerr << "GPDD has not been expanded." << endl;
		exit(1);
	}

	if(!caled) {
		cerr << "No Original Results." << endl;
		exit(1);
	}
	
	if(!anaAC->isSet()) {
		cout << "AC Analysis haven't been setted." << endl;
		exit(1);
	}
	
	vector<double> magTest, phaseTest;

	delPreProcess();
	complex<double> res;
	const vector<double>& allFreq = anaAC->sweep();
	vector<double>::const_iterator it = allFreq.begin();
	while(it != allFreq.end()) {
		double freq = *it;
		delUpdateSymbol(freq);
		res = delEvaluate();
		double mag = abs(res);
		double phase = arg(res) / PI * 180;
		magTest.push_back(20 * log(mag) / log(10));
		phaseTest.push_back(phase);
		++it;
	}
	
	if(fout) {
		string outFile = anaAC->outFile() + '_' + filename;

		ofstream out;
		out.open(outFile.c_str());
		if(!out) {
			cerr << "Cannot Open OutFile." << endl;
			exit(1);
		}
		out.setf(ios::scientific | ios::left);
		out.precision(8);
		for(int i = 0; i < allFreq.size(); i++) {
			out << setw(20) << allFreq[i] << setw(20) << magTest[i] << setw(20) << phaseTest[i] << endl;
		}
		out.close();
	}
	double magError = errorAssess(magList, magTest, 40);
	double phaseError = errorAssess(phaseList, phaseTest, 40);
	return pair<double, double>(magError, phaseError);
}

double GPDD::errorAssess(const vector<double>& ori, const vector<double>& test, int pos) {
	return (abs(ori[pos] - test[pos]) / ori[pos]);
}

double GPDD::errorAssess(const vector<double>& ori, const vector<double>& test) {
	if(ori.size() != test.size()) return -1;

	double res = 0;
	vector<double> error;
	vector<double>::const_iterator itOri = ori.begin();
	vector<double>::const_iterator itTest = test.begin();
	while(itOri != ori.end()) {
		double dataOri = *itOri, dataTest = *itTest;
		error.push_back(dataTest - dataOri);
		itOri++; itTest++;
	}

	double firMoment = 0, secMoment = 0;
	int N = error.size();
	vector<double>::const_iterator it = error.begin();
	while(it != error.end()) {
		firMoment += *it;
		secMoment += (*it) * (*it);
		it++;
	}
	double mean = firMoment / N;
	double var = secMoment / N - mean * mean;
	double stdVar = sqrt(var);
	return abs(stdVar / mean);
}

double GPDD::errorMagPhase(double mag, double phase) {
//			return phase * phase;
//			return mag * mag;
			return mag * mag + phase * phase;
}

void GPDD::symbolAssess(Symbol* curSym) {
	curSym->rd = true;

	//Open Case
	curSym->so = true; bool oflag = true;
	pair<double, double> openError = delCalculation();
	double omag = openError.first, ophase = openError.second;
	if(isinf(omag) || isnan(omag)) oflag = false;
	if(isinf(ophase) || isnan(ophase)) oflag = false;

	//Short Case
	curSym->so = false; bool sflag = true;
	pair<double, double> shortError = delCalculation();
	double smag = shortError.first, sphase = shortError.second;
	if(isinf(smag) || isnan(smag)) sflag = false;
	if(isinf(sphase) || isnan(sphase)) sflag = false;

	int delMethod; // 0: keep 1:short 2:open
	if(!oflag && !sflag) delMethod = 0;
	else if (!oflag && sflag) delMethod = 1;
	else if (oflag && !sflag) delMethod = 2;
	else {
		double odis = errorMagPhase(omag, ophase);
		double sdis = errorMagPhase(smag, sphase);
		delMethod = (sdis < odis) ? 1 : 2;
	}

	string outStat = "Keep";
	if(delMethod == 1) outStat = "Short";
	if(delMethod == 2) outStat = "Open";

	cout << setw(12) << curSym->name << setw(12) << outStat
		 << setw(12) << ' ' << setw(12) << omag << setw(12) << ophase
		 << setw(12) << ' ' << setw(12) << smag << setw(12) << sphase << endl;
	curSym->delType = delMethod;
	if(!curSym->delType) curSym->errExtend = 0;
	else if(curSym->delType == 1) {
		curSym->errExtend = errorMagPhase(smag, sphase);
	} else {
		curSym->errExtend = errorMagPhase(omag, ophase);
	}
	curSym->rd = false;
}

void GPDD::AnaSContribution() {
	//Assess Contribution for each symbol
	list<Symbol*>::iterator it = symbolList.begin(); it++;
	cout << setw(12) << "Name" << setw(12) << "Status" << setw(12) << "Open" << setw(12) << "Mag" << setw(12) << "Phase" << setw(12) << "Short" << setw(12) << "Mag" << setw(12) << "Phase" << endl;
	int nElement = 0;
	while(it != symbolList.end()) {
		if((*it)->e->type == YZ) {
			Symbol* p = *it;
			while(p) {
				nElement++;
				symbolAssess(p);
				p = p->lumpedNext;
			}
		} else {
			nElement++;
			symbolAssess(*it);
		}
		it++;
	}

	//Sort Symbol Contribution
	it = symbolList.begin(); it++;
	string *nameList = new string[nElement];
	double *errorList = new double[nElement];
	int k = 0;
	while(it != symbolList.end()) {
		if((*it)->e->type == YZ) {
			Symbol* p = *it;
			while(p) {
				nameList[k] = p->name;
				errorList[k] = p->errExtend;
				p = p->lumpedNext; k++;
			}
		} else {
			nameList[k] = (*it)->name;
			errorList[k] = (*it)->errExtend;
			k++;
		}
		it++;
	}
	double tmpError;
	string tmpName;
	for(int i = 0; i < nElement; i++) {
		for(int j = i + 1; j < nElement; j++) {
			if(errorList[i] > errorList[j]) {
				tmpError = errorList[i]; tmpName = nameList[i];
				errorList[i] = errorList[j]; nameList[i] = nameList[j];
				errorList[j] = tmpError; nameList[j] = tmpName;
			}
		}
	}
	cout << endl << "Sorted Error" << endl;
	for(int i = 0; i < nElement; i++) {
		cout << nameList[i] << '\t' << errorList[i] << endl;
	}

	//Evaluate Deletion Effect
	vector<Symbol*> sortedSymList;
	int step = 3; int nStart = 0; bool f = true;
	for(int n = 0; n <= nElement; n += step) {
		for(int i = nStart; i < n; i++) {
			it = symbolList.begin(); it++;
			while(it != symbolList.end()) {
				if((*it)->e->type == YZ) {
					Symbol* p = *it; bool stop = false;
					while(p) {
						if(p->name == nameList[i]) {
							if(p->delType) {
								p->rd = true;
								p->so = (p->delType == 2) ? true : false;
							}
							sortedSymList.push_back(p);
							stop = true; break;
						}
						p = p->lumpedNext;
					}
					if(stop) break;
				} else {
					if((*it)->name == nameList[i]) {
						if((*it)->delType) {
							(*it)->rd = true;
							(*it)->so = ((*it)->delType == 2) ? true : false;
						}
						sortedSymList.push_back(*it);
						break;
					}
				}
				it++;
			}
		}
		nStart = n;
		ostringstream ost;
		ost << n;
		string fname(ost.str());
		delCalculation(true, fname);

		if(f && (n + step > nElement)) {
			n = nElement - step;
			f = false;
		}
	}
	it = symbolList.begin();
	sortedSymList.push_back(*it);

	//Topology Analysis
	
	topologyAnalysis(sortedSymList);
	
}

void GPDD::topologyAnalysis(const vector<Symbol*> &sortSymList) {
	cout << "Topology Analysis Started..." << endl;

	//Initial Graph Generation
	Graph* tmpG = new Graph(numNode);
	list<Symbol*>::const_iterator itN = nullorList.begin();
	while(itN != nullorList.end()) {
		tmpG->addEdge((*itN)->pe);
		tmpG->addEdge((*itN)->e);
		itN++;
	}
	vector<Symbol*>::const_iterator it = sortSymList.begin();
	while(it != sortSymList.end()) {
		if((*it)->e->type == YZ) tmpG->addEdge((*it)->e);
		else {
			tmpG->addEdge((*it)->e);
			tmpG->addEdge((*it)->pe);
		}
		it++;
	}
	tmpG->shortNullor();

	vector<Graph*> simpGraphList;
	simpGraphList.push_back(tmpG);

	//Simplification
	it = sortSymList.begin(); Graph* preG = tmpG;
	while(it != sortSymList.end()) {
		Graph* newG = new Graph(preG); preG = newG;
		simpGraphList.push_back(newG);

		bool fakeSign;
		Symbol* curSym = *it;
		if(curSym->delType == 1) {
			if(curSym->e->type == YZ) newG->Short(curSym->e, fakeSign);
			else {
				newG->Short(curSym->e, fakeSign);
				newG->Short(curSym->pe, fakeSign);
			}
		} else if(curSym->delType == 2) {
			if(curSym->e->type == YZ) newG->Open(curSym->e);
			else {
				if(curSym->e->type = VC) newG->Open(curSym->e);
				else if(curSym->e->type = CC) newG->Short(curSym->e, fakeSign);
				else {
					cout << "Wrong Situation" << endl; break;
				}
				if(curSym->pe->type = CS) newG->Open(curSym->pe);
				else if(curSym->pe->type = VS) newG->Short(curSym->pe, fakeSign);
				else {
					cout << "Wrong Situation" << endl; break;
				}
			}
		} else {
			cout << "Didn't Consider Now." << endl;
			break;
		}

		it++;
	}

	int showNum;
	cin >> showNum;
	simpGraphList[showNum]->printGraph();
}

void GPDD::printSymbol() const {
	cout << "Print All Symbols in GPDD..." << endl;
	cout << "All Nullors:" << endl;
	list<Symbol*>::const_iterator it = nullorList.begin();
	while(it != nullorList.end()) {
		cout << (*it)->name << '\t' <<	(*it)->e->node1 << '\t'
			 << (*it)->e->node2 << '\t' <<	(*it)->pe->node1 << '\t'
			 << (*it)->pe->node2 << endl;
		++it;
	}
	cout << "All Symbols:" << endl;
	it = symbolList.begin();
	while(it != symbolList.end()) {
		if((*it)->e->type == YZ) {
			Symbol* tmp = *it;
			while(tmp) {
				cout << tmp->name << '\t' << tmp->v << '\t' 
					 << tmp->e->node1 << '\t' << tmp->e->node2 << endl;
				tmp = tmp->lumpedNext;
			}
		} else {
			cout << (*it)->name << '\t' <<	(*it)->v << '\t'
				 << (*it)->e->node1 << '\t' <<	(*it)->e->node2
				 << (*it)->pe->node1 << '\t' << (*it)->pe->node2 << endl;
		}
		++it;
	}
	cout << "The number of symbol EFH is " << maxEFH << "." << endl;
	cout << "Symbol Print Complete." << endl;
}

void GPDD::printInit() const {
	cout << "Initial State Printing..." << endl;
	cout << "GPDDRoot Information:" << endl;
	cout << "Symbol:" << GPDDRoot->sym->name << endl;
	cout << "LeftGraph:" << endl;
	GPDDRoot->leftG->printGraph();
	cout << "RightGraph:" << endl;	
	GPDDRoot->rightG->printGraph();
	graphHash.Print();
	GPDDNodeHash.Print();
	if(GPDDZero && GPDDOne) cout << "GPDDZero & GPDDOne have memory allocation." << endl;
	else cout << "GPDDZero & GPDDOne don't have memory allocation." << endl;
	cout << "Initial State Printing Complete." << endl;
}

void GPDD::AnalysisPrint() const {
	cout << endl << "GPDD Information:" << endl;

	cout << "Time Analysis:" << endl;
	cout << "Circuit Class to GPDD Class:\t" << cirTime << "usec." << endl;
	if(inited) cout << "GPDD Class Initialization:\t" << initTime << "usec." << endl;
	if(expanded) cout << "GPDD Class Expandsion Process:\t" << expandTime << "usec." << endl;
	if(stepExpand) cout << "Expansion with Step Analysis." << endl;
	if(reduceTime) cout << "GPDD Class Reduction Process:\t" << reduceTime << "usec." << endl;
	if(caled) {
		cout << endl;
		cout << "Average GPDD Calculation:\t" << onceAvgCalTime << "usec." << endl;
		cout << "Whole GPDD Calculation:  \t" << wholeCalTime << "usec." << endl;
	}
	cout << endl;
	cout << "The Total Time Cosuming:\t" << tTime << "usec." << endl;

	cout << endl << "Memory Analysis:" << endl;
	if(expanded) {
		cout << "The Number of Graph Before Reduction:\t" << graphNode[0] << endl;
		cout << "Hit Time of Graph Before Reduction:\t" << graphNode[1] << endl;
		cout << "Crash Time of Graph Before Reduction:\t" << graphNode[2] << endl;
		cout << "The Number of GPDDNode Before Redction:\t" << expandNode[0] << endl;
		cout << "Hit Time of GPDDNode Before Reduction:\t" << expandNode[1] << endl;
		cout << "Crash Time of GPDDNode Before Redction:\t" << expandNode[2] << endl;
	}
	if(reduceTime) {
		cout << endl;
		cout << "The Number of GPDDNode After Reduction:\t" << reduceNode[0] << endl;
		cout << "Hit Time of GPDDNode After Reduction:\t" << reduceNode[1] << endl;
		cout << "Crash Time of GPDDNode After Reduction:\t" << reduceNode[2] << endl;
	}

	if(termCounted) {
		cout << endl << "Term Analysis:" << endl;
		cout << setiosflags(ios::scientific) << setprecision(5);
		cout << "# of terms on numerator:\t" << double(nTermNum) << endl;
		cout << "# of terms on denominator:\t" << double(nTermDen) << endl;
	}
}

GPDD::~GPDD() {
	for(list<Symbol*>::iterator it = symbolList.begin(); 
		it != symbolList.end(); it++) {
		Symbol *tmp = (*it)->lumpedNext, *tmp0;
		while(tmp) {
			tmp0 = tmp;
			tmp = tmp->lumpedNext;
			delete tmp0;
		}
		delete (*it);
	}
	symbolList.clear();
	for(list<Symbol*>::iterator it = nullorList.begin(); 
		it != nullorList.end(); it++) delete (*it);
	nullorList.clear();

	delete GPDDZero;
	delete GPDDOne;
}
