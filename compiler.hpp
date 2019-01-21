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
#include <queue>
#include <algorithm>
#include <typeinfo>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

////////////////////
// STRUCTURES
////////////////////

typedef struct {
	string name;
    string type; //NUM, IDE, ARR
} Identifier;

typedef struct {
	long long int begin;
    long long int end;
} LongArray;

///////////////////////////
// GLOBAL VARIABLES
///////////////////////////

extern map <string, Identifier> identifierStack;
extern map<string, string> registerMap;
extern stack <Identifier> ideStack;
extern vector<string> codeStack;
extern stack <string> regStack;
extern stack <int> jumpStack;
extern vector <string> unChangeableIden;
extern stack <int> jumpStackForLoop;
extern stack <int> jumpStackForElIf;
extern stack<Identifier> endPointStack;
extern vector <string> unDeclaredVariables;

/////////////////////////////////
// bison
////////////////////////////////
int yylex();
extern int yylineno;
int yyerror(const string str);

void pushCommand(string str);
void pushCommandTemp(string command);
void createIdentifier(Identifier *s, string name, string type);
void printCodeStd();
void printCodeToFile(string FileOut);

///////////////////////////////////////
// number and identifier get functions
//////////////////////////////////////
void getNumber(string num);
void getIdentifier(string ide, int yylineno);
void getArrayWithNum(string ide, string place);
void getArrayWithIde(string ide, string place);

///////////////////////////////////
// simple expression functions
//////////////////////////////////
void declarationIde(string ide, int yylineno);
void declarationArray(string ide,string num1, string num2, int yylineno);
void expressRead();
void expressWrite();
void ideAsignExpress(string ide, int yylineno);
void makeShiftOnTable(string lastS, string ide);

///////////////////////////////////
// loop expressions functions
/////////////////////////////////
void customFor(string iterator, string endpoint);
void customForDeclaration(string ide, int yylineno);
void downtoFor(string iterator, string endpoint);
void downtoForDeclaration(string ide, int yylineno);

void customIf();
void ifCondition();
void elseIf();
void elseInIf();
void customWhile();
void customWhileDeclaration();
void customDoWhile();
void doWhileBegins();

//////////////////////////////////
// memory managment functions
/////////////////////////////////
void setRegister(string reg, long long int value);
void storeInMemory(string reg, string variable);
void loadFromMemory(string variable, string reg);
int findInVector(string var);
int findTableBeginning(string var);

/////////////////////////////
// operation functions
////////////////////////////
void add(string ide1, string ide2, int yylineo);
void sub(string ide1, string ide2, int yylineo);
void multp(string ide1, string ide2, int yylineo);
void multpPush();
void dividePush();
void divide(string ide1, string ide2, int yylineo);
void modulo(string ide1, string ide2, int yylineo);
void moduloPush();

////////////////////////////////
// conditions functions
///////////////////////////////
void equalCondition(string ide1, string ide2, int yylineno);
void notEqualCondition(string ide1, string ide2, int yylineno);
void greaterCondition(string ide1, string ide2, int yylineno);
void lowerCondition(string ide1, string ide2, int yylineno);
void greaterEqualCondition(string ide1, string ide2, int yylineno);
void lowerEqualCondition(string ide1, string ide2, int yylineno);

