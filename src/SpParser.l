%option noyywrap case-insensitive
%{
#include <cstring>
#include "SpParser.hpp"
#define YY_DECL int yylex(yy::SpParser::semantic_type *yylval, yy::SpParser::location_type *yylloc)

namespace yy {
	int column = 1;
	FILE* curFile = yyin;
}

#define YY_USER_ACTION {if(yy::curFile != yyin) { \
                            yylineno = 1; \
                            yy::curFile = yyin; \
                        } \
                        if(yylineno != yylloc->begin.line) yy::column = 0; \
						yylloc->begin.line = yylineno; \ 
						yylloc->begin.column = yy::column; \
						yy::column = yy::column+yyleng; \
						yylloc->end.column = yy::column; \
						yylloc->end.line = yylineno;}
typedef yy::SpParser::token token;
%}

/* Defining output file name */
%option outfile="SpScanner.cpp"	

/* Defining Regular Expressions of the patterns */
COMMENT			[\*][^\n]+
ALPHA			[A-Za-z_]
DIGIT			[0-9]
ALPHANUM		[A-Za-z_0-9]
STRING			{ALPHANUM}+
INTEGER			{DIGIT}+
FLOAT			[\-]?{DIGIT}+"."{DIGIT}+([Ee][\+\-]?{DIGIT}+)?
RESISTOR		[Rr]{ALPHANUM}+
CAPACITOR		[Cc]{ALPHANUM}+
INDUCTOR		[Ll]{ALPHANUM}+
CURRENTSRC		[Ii]{ALPHANUM}+
VOLTAGESRC		[Vv]{ALPHANUM}+
VCVS			[Ee]{ALPHANUM}+
VCCS			[Gg]{ALPHANUM}+
CCCS			[Ff]{ALPHANUM}+
CCVS			[Hh]{ALPHANUM}+
NULLOR			[Nn]{ALPHANUM}+
EOL				[\n]
DELIMITER		[ \t]+
UNIT			[Ff]|[Pp]|[Nn]|[Uu]|[Mm]|[Kk]|[Mm][Ee][Gg]|[Gg]|[Tt]|[Aa]
VALUE			({FLOAT}|[-]?{INTEGER}){UNIT}
END				[\.][Ee][Nn][Dd]
ACANALYSIS		[\.][Aa][Cc]

%%

%{
   yylloc->step ();
%}

{ACANALYSIS}	{return token::ACANALYSIS;}
{INTEGER}		{yylval->n = atoi(yytext); return token::INTEGER;}
{FLOAT}			{yylval->f = atof(yytext); return token::FLOAT;}
{RESISTOR}		{yylval->s = strdup(yytext); *yylval->s = toupper(*yylval->s); return token::RESISTOR;}
{CAPACITOR}		{yylval->s = strdup(yytext); *yylval->s = toupper(*yylval->s); return token::CAPACITOR;}
{INDUCTOR}		{yylval->s = strdup(yytext); *yylval->s = toupper(*yylval->s);  
	 			 if(!strcasecmp(yylval->s,"LIN")) return token::LIN;
                 return token::INDUCTOR;
				}
{CURRENTSRC}	{yylval->s = strdup(yytext); *yylval->s = toupper(*yylval->s); return token::CURRENTSRC;}
{VOLTAGESRC}	{yylval->s = strdup(yytext); *yylval->s = toupper(*yylval->s); return token::VOLTAGESRC;}
{VCVS}			{yylval->s = strdup(yytext); *yylval->s = toupper(*yylval->s); return token::VCVS;}
{VCCS}			{yylval->s = strdup(yytext); *yylval->s = toupper(*yylval->s); return token::VCCS;}
{CCCS}			{yylval->s = strdup(yytext); *yylval->s = toupper(*yylval->s); return token::CCCS;}
{CCVS}			{yylval->s = strdup(yytext); *yylval->s = toupper(*yylval->s); return token::CCVS;}
{NULLOR}		{yylval->s = strdup(yytext); *yylval->s = toupper(*yylval->s); return token::NULLOR;}
{VALUE}			{double value;
				 value = atof(yytext);
				 int len = strlen(yytext);
				 char u = yytext[len - 1];
				 u = tolower(u);
				 switch(u) {
					 case 'a': yylval->f = value * 1e-18; break;
					 case 'f': yylval->f = value * 1e-15; break;
					 case 'p': yylval->f = value * 1e-12; break;
					 case 'n': yylval->f = value * 1e-9; break;
					 case 'u': yylval->f = value * 1e-6; break;
					 case 'm': yylval->f = value * 1e-3; break;
					 case 'k': yylval->f = value * 1e3; break;
					 case 'g': yylval->f = value * 1e9;
							   if((yytext[len - 2] == 'E' || yytext[len - 2] == 'e')
							   && (yytext[len - 3] == 'M' || yytext[len - 3] == 'm'))
								   yylval->f *= 1e-3;
			 				   break;
					 case 't': yylval->f = value * 1e12; break;
					 default:  yylval->f = value;
				 }
				 return token::VALUE;
				}
{STRING}		{yylval->s = strdup(yytext);
				 if(!strcasecmp(yylval->s, "OCT")) return token::OCT;
				 if(!strcasecmp(yylval->s, "DEC")) return token::DEC;
                 return token::STRING;
			    }
{EOL}		    {return token::EOL;}
<<EOF>>			{yyterminate();}
{DELIMITER}		{yylloc->step();}
{COMMENT}		{yylloc->step();}
{END}			{return token::END;}
%%
