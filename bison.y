%{
#include "compiler.hpp"

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
%token <str> WRITE READ IDE SEM TO DOWNTO
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
    | IF condition THEN commands ELSE commands ENDIF {
        elseIf();
    }
    | WHILE condition DO commands ENDWHILE {
        customWhile();
    }
    | DO commands WHILE condition ENDDO {
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

    value EQ value
    | value NE value
    | value LT value
    | value GT value
    | value LE value
    | value GE value

;

value:
    NUM {
        getNumber($1);  	
    }
    | identifier
    ;

identifier:

    IDE {
        getIdentifier($1);
      	
    }
    | IDE LB NUM RB
    | IDE LB IDE RB
    ;

%%


void parser(long long int argv, char* argc[]) {
	
	yyparse();

    string file = "";
   
        printCodeStd();
    
}

int main(int argv, char* argc[]){
	parser(argv, argc);
	return 0;
}

int yyerror(string str){
    cout << "Błąd [okolice linii " << yylineno << \
    "]: " << str << endl;
    exit(1);
}
