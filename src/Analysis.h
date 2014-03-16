#ifndef  ANALYSIS_H
#define  ANALYSIS_H

#include <cmath>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

enum ACTYPE {LIN, DEC, OCT};

class Analysis {
	private:
		bool set;
		int acType;
		int numStep;
		double fstart;
		double fend;
		string outFileName;
		vector<double> allFreq;
		string ACTypeTranslate(int type) const;
		bool compareDouble(double a, double b) const;

	public:
		Analysis();
		Analysis(const Analysis& other);
		~Analysis() {}
		void ACInitial(int type, int num, double start, double end);
		void setOutFile(const char* fname);
		void printACInf() const;
		const vector<double>& sweep();
		inline bool isSet() {return set;}
		inline string outFile() {return outFileName;}
};

#endif  /*ANALYSIS_H*/
