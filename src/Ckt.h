#ifndef  CKT_H
#define  CKT_H

#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <cstdlib>
#include "Analysis.h"

using namespace std;

enum EdgeType {YZ = 0, CC, VC, NU, CS, VS, NO};

struct cktNode {
	string name;
	int index;
};

struct cktEdge {
	string name;
	string vname;
	cktNode* node1;
	cktNode* node2;
	double val;
	int type;
	cktEdge* pairEdge;
	bool Vused;
};

class Ckt {
	private:
		int numNode;
		int numEdge;
		bool src;	//src appear
		bool out;	//out appear
		list<cktNode*> nodeList;
		list<cktEdge*> edgeList;
		
		Analysis* anaAC;
	
		//auxiliary function
		cktEdge* findEdge(const char *eName);
		cktEdge* findCCEdge(const char *eName);
		cktEdge* findVName(cktEdge* VEdge);
		cktNode* insertNode(const char *nName);
		void numberNode(); 
		string edgeTypeTranslate(int type) const;
		int nodeTypeTranslate(cktNode* node) const;
		void removeAllV();
		bool check_pair();

	public:
		//constructer for Ckt
		Ckt();
		Ckt(const char* fname);

		//function for parser
		void parseRLC(const char* eleName, const char* nName1, const char* nName2, double v);
		void parseVCXS(const char* eleName, const char* XSName1, const char* XSName2, const char* VCName1, const char* VCName2, double v);
		void parseCCXS(const char* eleName, const char* XSName1, const char* XSName2, const char* VName, double v);
		void parseVsrc(const char* eleName, const char* nName1, const char* nName2, double v);
		void parseIsrc(const char* eleName, const char* nName1, const char* nName2, double v);
		void parseOutVC(const char* nName1, const char* nName2);
		void parseOutCC(const char* volName);
		void parseAC(int type, int step, double s, double e);

		//print function
		void printCkt() const;

		//interface for GPDD class
		inline int numN() const {return numNode;} 
		inline int numE() const {return numEdge;}
		inline bool pairInOut() const {return src && out;}
		inline const list<cktEdge*> &eList() const {return edgeList;}
		inline Analysis* anaCkt() const {return anaAC;}

		//deconstructor
		~Ckt();
};

#endif  /*CKT_H*/
