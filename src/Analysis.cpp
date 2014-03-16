#include "Analysis.h"

Analysis::Analysis() {	
	set = false;
	acType = LIN;
	numStep = 0;
	fstart = 0;
	fend = 0;
	allFreq.clear();
}

void Analysis::ACInitial(int type, int num, double start, double end) {
	set = true;
	acType = type;
	numStep = num;
	fstart = start;
	fend = end;
}

void Analysis::setOutFile(const char* fname) {
	string tmp = string(fname);
	size_t loc = tmp.rfind('.');
	if(loc == string::npos) tmp += ".out";
	else if(tolower(tmp[loc + 1]) < 'a' || tolower(tmp[loc + 1]) > 'z') tmp += ".out";
	else tmp.replace(loc, tmp.length() - loc, ".out");
	outFileName = tmp;
}

string Analysis::ACTypeTranslate(int type) const{
	string tmp;
	switch (type) {
		case LIN: tmp = "LIN"; break;
		case DEC: tmp = "DEC"; break;
		case OCT: tmp = "OCT"; break;
		default: tmp = "Impossible";
	}
	return tmp;
}

void Analysis::printACInf() const {
	cout << "AC Infomation:" << endl;
	if(set) {
		cout << "Output File:" << outFileName << endl;
		cout << "Sweep Type:" << ACTypeTranslate(acType) << endl;
		cout << "Step Number:" << numStep << endl;
		cout << "Freq Start:" << fstart << endl;
		cout << "Freq End:" << fend << endl;
	}else cout << "No setting of AC Analysis." << endl;
}

bool Analysis::compareDouble(double a, double b) const {
	const double e = 1e-6;
	if(a < b) {
		if(abs(a - b) / b < e) return false;
		else return true;
	} else return false;
}

const vector<double>& Analysis::sweep() {
	allFreq.clear();
	if(!isSet()) return allFreq;
	if(numStep < 1 || fstart >= fend) {
		allFreq.push_back(fstart);
		return allFreq;
	}
	switch (acType) {
		case LIN: { if(numStep == 1) {
						allFreq.push_back(fstart);
						return allFreq;
					}
					double delta = (fend - fstart) / (numStep - 1);
					for(double x = fstart; x <= fend; x += delta)
						allFreq.push_back(x);
					if(compareDouble(*allFreq.rbegin(), fend)) allFreq.push_back(fend);
				  }	break;
		case OCT: { double mul = pow(8, 1.0 / numStep);
					for(double x = fstart; x <= fend; x *= mul)
						allFreq.push_back(x);
					if(compareDouble(*allFreq.rbegin(), fend)) allFreq.push_back(fend);
				  }	break;
		case DEC: { double mul = pow(10, 1.0 / numStep);
					for(double x = fstart; x <= fend; x *= mul)
						allFreq.push_back(x);
					if(compareDouble(*allFreq.rbegin(), fend)) allFreq.push_back(fend);
				  } break;
		default: break;
	}
	return allFreq;
}
