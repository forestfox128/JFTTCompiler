#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <stack>
#include <vector>
#include <algorithm>
using namespace std;

////////////////////
// STRUCTURES
////////////////////

typedef struct {
	string name;
    string type; //NUM, IDE, ARR
    int initialized;
    int counter;
	long long int local;
  	long long int tableSize;
} Identifier;

///////////////////////////
// GLOBAL VARIABLES
///////////////////////////

extern map<string, Identifier> identifierStack;
extern map<string, string> registerMap;
extern stack <Identifier> ideStack;
extern vector<string> codeStack;
extern stack <string> regStack;

int yylex();
extern int yylineno;
int yyerror(const string str);

void pushCommand(string str);

void initializeStackReg();
void pushCommandOneArg(string str, long long int num);
void createIdentifier(Identifier *s, string name, long long int isLocal,long long int isArray, string type);
void insertIdentifier(string key, Identifier i);

void addDeclaredVariable(string variable, Identifier i);
Identifier getValueIndentifier(string name);
void removeIdentifier(string key);
string setRegister(string number);
string getRegister();
void add(string ide1, string ide2, int yylineo);
void sub(string ide1, string ide2, int yylineo);
void multp(string ide1, string ide2, int yylineo);
void divide(string ide1, string ide2, int yylineo);
void printCodeStd();

void declarationIde(string ide, int yylineno);
void expressRead();
void expressWrite();
void ideAsignExpress(string ide);
void getNumber(string num);
void getIdentifier(string ide);
