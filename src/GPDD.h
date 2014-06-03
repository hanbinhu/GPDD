#ifndef  GPDD_H
#define  GPDD_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <list>
#include <stack>
#include <queue>
#include <complex>
#include <sys/time.h>
#include "GPDDNode.h"
#include "HashGraph.h"
#include "HashGPDDNode.h"
#include "HashReduction.h"

using namespace std;

class GPDD {
	private:
		//For Calculation
		list<Symbol*> symbolList;
		list<Symbol*> nullorList;

		int numNode;
		int numEdge;
		int maxEFH;
		
		GPDDNode* GPDDRoot;
		GPDDNode* GPDDZero;
		GPDDNode* GPDDOne;

		HashGraph graphHash;
		HashGPDDNode GPDDNodeHash;
		HashReduction ReductionHash;

		Analysis *anaAC;

		//For flag
		bool inited;
		bool expanded;
		bool caled;

		bool stepExpand;

		bool termCounted;

		//Time Record
		long cirTime;
		long initTime;
		long expandTime;

		long reduceTime;

		long onceAvgCalTime;
		long wholeCalTime;

		long tTime;

		//Space Record
		//0 for NodeNum
		//1 for Hit
		//2 for Crash
		long graphNode[3];
		long expandNode[3];
		long reduceNode[3];

		//Term Count Information
		long long nTermNum;
		long long nTermDen;

		//Const
		const static double PI = 3.14159265358979323846264338327950288;
		const static long TIME_UNIT = 1000000;

		//Private Function
		pair<GPDDNode*, bool> Include(const GPDDNode* fNode);
		pair<GPDDNode*, bool> Exclude(const GPDDNode* fNode);

		void printConnection(const GPDDNode* curNode) const;
		void stepExpandPrint(const GPDDNode* curNode) const;

		complex<double> Evaluate() const;
		void updateSymbol(const double freq);

		void delPreProcess();
		void delUpdateSymbol(const double freq);
		complex<double> EvaluatePC(const GPDDNode* p, const GPDDNode* c) const;
		complex<double> delEvaluate() const;
		pair<double, double> delCalculation(bool fout = false, const string& filename = "");

		double errorAssess(const vector<double>& ori, const vector<double>& test);
		double errorAssess(const vector<double>& ori, const vector<double>& test, int pos);
		double errorMagPhase(double mag, double phase);
		void symbolAssess(Symbol* curSym);
		void topologyAnalysis(const vector<Symbol*> &sortSymList);

		vector<double> magList;
		vector<double> phaseList;
		int GBW;

	public:
		//construction
		GPDD(const Ckt* myCkt);

		//Initializtion
		void init();

		//Expand
		void expand(bool step = false);
		void reduction();

		//Evaluation
		void Calculation();
		void TermCount();

		//Symbol Contribution Analysis
		void AnaSContribution();

		//For print
		void printSymbol() const;
		void printInit() const;
		void printGPDD() const;
		void AnalysisPrint() const;

		//deconstruction
		~GPDD();
};

#endif  /*GPDD_H*/
