%{
#include "compiler.hpp"

extern FILE *yyin;
extern FILE *yyout;

%}



%define parse.error verbose
%define parse.lac full

%union {
    char* str;
    long long int num;
}
%token <str> NUM
%token <str> DECLARE IN END IF THEN ELSE ENDIF
%token <str> WHILE DO ENDWHILE FOR FROM ENDFOR ENDDO
%token <str> WRITE READ IDE SEM TO DOWNTO COL
%token <str> LB RB ASG EQ LT GT LE GE NE ADD SUB MUL DIV MOD

%type <str> value
%type <str> identifier


%%
program:
    DECLARE declarations IN commands END {
            pushCommand("HALT");
    }
;

declarations:
    declarations IDE SEM {
        declarationIde($2, yylineno);
    }
    | declarations IDE LB NUM COL NUM RB SEM {
        declarationArray($2,$4,$6, yylineno);
    }
    |
;
commands:

    commands command
|   command
;

command:

    READ expression SEM {
        expressRead();
    }

    | identifier ASG expression SEM {
        ideAsignExpress($1, yylineno);   
    }

    | WRITE expression SEM {
        expressWrite();
    }

    | FOR IDE FROM value TO value DO { customForDeclaration($2, yylineno); } commands ENDFOR {
        customFor($2, $6);
    }
    | FOR IDE FROM value DOWNTO value { downtoForDeclaration($2, yylineno); } DO commands ENDFOR {
        downtoFor($2, $6);
    }
    | IF condition THEN commands ENDIF {
        customIf();
    }
    | IF condition THEN commands ELSE {elseInIf();} commands ENDIF {
        elseIf();
    }
    | WHILE {customWhileDeclaration(); } condition DO commands ENDWHILE {
        customWhile();
    }
    | DO {doWhileBegins();}commands WHILE condition ENDDO {
        customDoWhile();
    }

    
    ;

expression:

     value ADD value {
        add($1, $3, yylineno);
     }

    | value SUB value {
        sub($1, $3, yylineno);
     }

    | value MUL value {
        multp($1, $3, yylineno);
    }

    | value DIV value {
        divide($1, $3, yylineno);
    }
    | value MOD value {
        modulo($1, $3, yylineno);
    }
    
    | value
    ;

condition:

    value EQ value {
        equalCondition($1, $3, yylineno);
    }
    | value NE value {

        notEqualCondition($1, $3, yylineno);
    }
    | value LT value {
        lowerCondition($1, $3, yylineno);
    }
    | value GT value {
        greaterCondition($1, $3, yylineno);

    }
    | value LE value{
        lowerEqualCondition($1, $3, yylineno);
    }
    | value GE value{
        greaterEqualCondition($1, $3, yylineno);
    }

;

value:
    NUM {
        getNumber($1);  	
    }
    | identifier
    ;

identifier:

    IDE {
        getIdentifier($1,yylineno);
      	
    }
    | IDE LB NUM RB {
        getArrayWithNum($1,$3);
    }
    | IDE LB IDE RB {
        getArrayWithIde($1,$3);
    }
    ;

%%

void printCodeToFile(string FileOut){

    ofstream fileO;
    fileO.open (FileOut);
    long long int i;
    for (i = 0; i < codeStack.size(); i++){
        
            fileO << codeStack.at(i) <<"\n";   
    } 
    fileO.close();
}

void parser(long long int argv, char* argc[]) {
	
    if (argv == 3) {
        string outFile(argc[2]);
        yyin = fopen(argc[1], "r");
        yyparse();
        printCodeToFile(outFile);
    }
    else{
        cerr<<"ERROR: Niepoprawne wywołanie programu";
    }
	  
}

int main(int argv, char* argc[]){
	parser(argv, argc);
	return 0;
}

int yyerror(string str){
    cerr << "ERROR <line " << yylineno << ">: " << str << endl;
    exit(1);
}
