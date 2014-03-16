/*%language "C++"*/
%skeleton "lalr1.cc"
%defines
%locations

%define "parser_class_name" "SpParser"

%code requires{
#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <cstring>
#include <string>
#include "Ckt.h"
using namespace std;
extern FILE* yyin;
}

/* set the output file name */
%output="./SpParser.cpp"
%verbose

%union
{
	double f;
	int n;
	char* s;
};

/* defining parser input variable type */
%parse-param{Ckt* myCkt}

/* assigning data type for tokens and patterns */
%token<f> FLOAT
%token<f> VALUE
%token<n> INTEGER
%token<s> STRING 
%token<s> RESISTOR CAPACITOR INDUCTOR CURRENTSRC VOLTAGESRC
%token<s> NULLOR VCVS VCCS CCCS CCVS 
%token<s> DEC LIN OCT
%token END EOL ACANALYSIS
%type<s> resistor capacitor inductor currentsrc voltagesrc 
%type<s> nullor vcvs vccs cccs ccvs 
%type<s> node variable 
%type<n> actype
%type<f> value

%{
extern int yylex(yy::SpParser::semantic_type* yylval,
                 yy::SpParser::location_type* yylloc);
%}

%initial-action {
  // Filename for locations here
  @$.begin.filename = @$.end.filename = new std::string("stdin");
}

%%

spice: netlist end
;

end: end EOL
   | END EOL
   | END
;

netlist: netlist line
       | line
;

line: component EOL
	| EOL
;

component: resistor
		 | capacitor
		 | inductor
		 | currentsrc
		 | voltagesrc
		 | nullor
		 | vcvs
		 | vccs
		 | cccs
		 | ccvs
		 | acanalysis
;

resistor: RESISTOR node node value
		  {
				myCkt->parseRLC($1, $2, $3, 1.0 / $4);
		  }
;

capacitor: CAPACITOR node node value					
           {
				myCkt->parseRLC($1, $2, $3, $4);
		   }
;		
	
inductor: INDUCTOR node node value  				
		  {
				myCkt->parseRLC($1, $2, $3, -1.0 / $4);
		  }
		| LIN node node value
		  {
				myCkt->parseRLC($1, $2, $3, -1.0 / $4);
		  }
;

voltagesrc: VOLTAGESRC node node value						
			{
				myCkt->parseVsrc($1, $2, $3, $4);
			}	
;

currentsrc: CURRENTSRC node node value						
            {
				myCkt->parseIsrc($1, $2, $3, $4);
			}
;

nullor: NULLOR node node node node
	    {
				myCkt->parseVCXS($1, $2, $3, $4, $5, 0);
		}

vcvs: VCVS node node node node value
	  {
				myCkt->parseVCXS($1, $2, $3, $4, $5, $6);
	  }
;

vccs: VCCS node node node node value
	  {
				myCkt->parseVCXS($1, $2, $3, $4, $5, $6);
	  }
;

cccs: CCCS node node VOLTAGESRC value
	  {
				myCkt->parseCCXS($1, $2, $3, $4, $5);
	  }
;

ccvs: CCVS node node VOLTAGESRC value
	  {
				myCkt->parseCCXS($1, $2, $3, $4, $5);
	  }
;

acanalysis: ACANALYSIS node node actype INTEGER value value
			{
				myCkt->parseOutVC($2, $3);
				myCkt->parseAC($4, $5, $6, $7);
			}
		  | ACANALYSIS VOLTAGESRC actype INTEGER value value
		    {
				myCkt->parseOutCC($2);
				myCkt->parseAC($3, $4, $5, $6);
			}
;

actype: DEC					{$$ = 1;}
	  | OCT					{$$ = 2;}
	  | LIN					{$$ = 0;}
;

node: variable				{$$ = strdup($1);}
    | INTEGER				{char s[64]; sprintf(s, "%d", $1); $$ = strdup(s);}
;

variable: STRING			{$$ = strdup($1);}
	    | RESISTOR			{$$ = strdup($1);}
	    | CAPACITOR			{$$ = strdup($1);}
        | INDUCTOR 			{$$ = strdup($1);}
	    | CURRENTSRC  		{$$ = strdup($1);}
	    | VOLTAGESRC		{$$ = strdup($1);}
	    | VCVS				{$$ = strdup($1);}
	    | VCCS				{$$ = strdup($1);}
	    | CCCS				{$$ = strdup($1);}
	    | CCVS				{$$ = strdup($1);}
		| NULLOR			{$$ = strdup($1);}
	    | DEC				{$$ = strdup($1);}
	    | OCT				{$$ = strdup($1);}
	    | LIN				{$$ = strdup($1);}
;

value: VALUE				{$$ = $1;}
	 | FLOAT				{$$ = $1;}
     | INTEGER				{$$ = $1;}
;
	
%%

void yy::SpParser::error(const location &loc, const string &s) 
{
	cerr << "error at " << loc << ": " << s << endl;
}
