#include <iostream>
#include <string>
#include <cstring>
#include "GPDD.h"

using namespace std;
bool input_bool();

int main(int argc, char *argv[])
{
	if(argc < 2) cout << "No input file" << endl;
	else {
		if(argc == 2) {
			Ckt *myCkt = new Ckt(argv[1]);
			GPDD *myGPDD = new GPDD(myCkt);
			myGPDD->init();
			myGPDD->expand();
			myGPDD->reduction();
			myGPDD->Calculation();
			myGPDD->TermCount();
			myGPDD->AnalysisPrint();
			myGPDD->AnaSContribution();
			delete myGPDD;
			delete myCkt;

		} else {
			bool flag;

			Ckt *myCkt = new Ckt(argv[1]);
			GPDD *myGPDD = new GPDD(myCkt);

			cout << "Init Information Print?[y/n]"; 
			flag = input_bool();
			if(flag) myCkt->printCkt();
			if(flag) myGPDD->printSymbol();
			myGPDD->init();
			if(flag) myGPDD->printInit();

			cout << "Expansion Step Analysis?[y/n]";
			flag = input_bool();
			myGPDD->expand(flag);

			cout << "Reduction or Not?[y/n]";
			flag = input_bool();
			if(flag) myGPDD->reduction();

			cout << "Print GPDD or Not?[y/n]";
			flag = input_bool();
			if(flag) myGPDD->printGPDD();

			myGPDD->Calculation();

			cout << "Calculate terms of GPDD or Not?[y/n]";
			flag = input_bool();
			if(flag) myGPDD->TermCount();

			myGPDD->AnalysisPrint();

			cout << "Calculate the Contribution of each symbol in GPDD?[y/n]";
			flag = input_bool();
			if(flag) myGPDD->AnaSContribution();

			delete myGPDD;
			delete myCkt;
		}
	}
	return 0;
}

bool input_bool() {
	char c, tmp; bool f;
	const string errMsg = "Wrong input, please input again[y/n]:";
	while(1) {
		f = false; 
		do {c = cin.get();} while(c == '\n' || c == '\t' || c == ' ');
		while((tmp = cin.get()) != '\n') {
			if(!f && tmp != ' ' && tmp != '\t') {
				f = true;
				break;
			}
		}
		if(f) cout << errMsg;
		else {
			if(c == 'y') return true;
			else if(c == 'n') return false;
			else cout << errMsg;
		}
	}
}
