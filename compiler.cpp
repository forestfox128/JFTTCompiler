#include "compiler.hpp"

//map<string, Identifier> identifierStack;
map<string, string> registerMap;
stack<Identifier> ideStack;
stack<Identifier> endPointStack;
stack <int> jumpStack;
stack <int> jumpStackForLoop;
stack <int> jumpStackForElIf;
vector<string> codeStack;
stack<string> regStack;
vector <string> memoryTable;
vector <string> unChangeableIden;
vector <string> setVariables;

int forCounter = 0;
long long int programCounter = 0;
long long int tempProgramCounter = 0;
LongArray longArray;
int longArr = 0;

///////////////////////////////////
// jump functions
//////////////////////////////////
void makeJump(){
    for(int i = codeStack.size() - 1; i >= 0; i--){
        if(codeStack.at(i) == "JUMP $mark"){
            string newJump = "JUMP "+ to_string(jumpStackForLoop.top());
            codeStack[i] = newJump;
            jumpStackForLoop.pop();
            return;
        }
    }       
}
void makeElfJump(){
    for(int i = codeStack.size() - 1; i >= 0; i--){
        
        if(codeStack.at(i) == "JUMP $elifmark"){
            string newJump = "JUMP "+ to_string(jumpStackForElIf.top());
            codeStack[i] = newJump;
            jumpStackForElIf.pop();
            return;
        }
    }       
}
////////////////////////////////////
// helper functions
////////////////////////////////////
bool is_number(const std::string& s){

    return !s.empty() && std::find_if(s.begin(), 
        s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
}

string getIdeFromBrackets(string str, char delimiter) {
  vector<string> internal;
  stringstream ss(str); // Turn the string into a stream.
  string tok;
  while(getline(ss, tok, delimiter)) {
    internal.push_back(tok);
  }
  int lastPos = internal.size() - 1;

  return internal[lastPos];
}

string getIdeFromArray(string str, char delimiter) {
  vector<string> internal;
  stringstream ss(str); // Turn the string into a stream.
  string tok;
  while(getline(ss, tok, delimiter)) {
    internal.push_back(tok);
  }
  int lastPos = internal.size() - 1;

  return internal[0];
}

void createLongArray(LongArray *s, long long int begin, long long int end)
{
    s->begin = begin;
    s->end = end;
}
void createIdentifier(Identifier *s, string name, string type)
{
    s->name = name;
    s->type = type;
}

void loadValueFromTable(string ide){
    string inBracket = getIdeFromBrackets(ide,'|');
    if(!is_number(inBracket)){
        makeShiftOnTable(inBracket, ide);
        pushCommand("LOAD B");
        // pushCommand("PUT B");
    }
    else{
        ide = getIdeFromArray(ide,'|') +'|'+ getIdeFromBrackets(ide,'|');
        loadFromMemory(ide,"B");
    }
}
void addO(Identifier a, Identifier b){
    if ((a.type == "IDE" && b.type == "NUM"))
    {
        loadFromMemory(a.name,"B");
        for (int i = 0; i < std::stoll(b.name); i++)
        {
            pushCommand("INC B");
        }
    }
    if (a.type == "NUM" && b.type == "IDE")
    {
        loadFromMemory(b.name,"B");
        for (int i = 0; i < std::stoll(a.name); i++)
        {
            pushCommand("INC B");
        }
    }
    if (a.type == "IDE" && b.type == "IDE")
    {
        loadFromMemory(a.name,"B");
        loadFromMemory(b.name,"C");
        pushCommand("ADD B C");
    }
    if(a.type == "NUM" && b.type == "NUM"){

        setRegister("B",std::stoll(a.name));
        setRegister("C",std::stoll(b.name));
        pushCommand("ADD B C");
    }
    if(a.type == "NUM" && b.type == "ARR"){
        loadValueFromTable(b.name);
        setRegister("C", stoll(a.name));
        pushCommand("ADD B C");
    }
    if(a.type == "IDE" && b.type == "ARR"){
        loadValueFromTable(b.name);
        loadFromMemory(a.name, "C");
        pushCommand("ADD B C");
    }
    if(a.type == "ARR" && b.type == "NUM"){
        loadValueFromTable(a.name);
        setRegister("C", stoll(b.name));
        pushCommand("ADD B C");
    }
    if(a.type == "ARR" && b.type == "IDE"){
        loadValueFromTable(a.name);
        loadFromMemory(b.name,"C");
        pushCommand("ADD B C");
    }
    if(a.type == "ARR" && b.type == "ARR"){
        loadValueFromTable(b.name);
        pushCommand("COPY E B");
        loadValueFromTable(a.name);
        pushCommand("ADD B E");
    }
}

void subO(Identifier a, Identifier b){
    if ((a.type == "IDE" && b.type == "NUM"))
    {
        loadFromMemory(a.name,"B");
        for (int i = 0; i < std::stoll(b.name); i++)
        {
            pushCommand("DEC B");
        }
    }
    if (a.type == "NUM" && b.type == "IDE")
    {
        loadFromMemory(b.name,"C");
        setRegister("B",std::stoll(a.name));
        pushCommand("SUB B C");
    }
    if (a.type == "IDE" && b.type == "IDE")
    {
        loadFromMemory(a.name,"B");
        loadFromMemory(b.name,"C");
        pushCommand("SUB B C");
    }
    if(a.type == "NUM" && b.type == "NUM"){

        setRegister("B",std::stoll(a.name));
        setRegister("C",std::stoll(b.name));
        pushCommand("SUB B C");
    }
    if(a.type == "NUM" && b.type == "ARR"){
        loadValueFromTable(b.name);
        setRegister("C", stoll(a.name));
        pushCommand("SUB C B");
        pushCommand("COPY B C");
    }
    if(a.type == "IDE" && b.type == "ARR"){
        loadValueFromTable(b.name);
        loadFromMemory(a.name, "C");
        pushCommand("SUB C B");
        pushCommand("COPY B C");
    }
    if(a.type == "ARR" && b.type == "NUM"){
        loadValueFromTable(a.name);
        setRegister("C", stoll(b.name));
        pushCommand("SUB B C");
    }
    if(a.type == "ARR" && b.type == "IDE"){
        loadValueFromTable(a.name);
        loadFromMemory(b.name,"C");
        pushCommand("SUB B C");
    }
    if(a.type == "ARR" && b.type == "ARR"){
        loadValueFromTable(b.name);
        pushCommand("COPY F B");
        loadValueFromTable(a.name);
        pushCommand("SUB B F");
    }
}

void multpO(Identifier a, Identifier b){

    if ((b.type == "IDE" && a.type == "IDE"))
    {
        loadFromMemory(a.name,"C");
        loadFromMemory(b.name,"B");
        multpPush();
    }

    if(a.type == "NUM" && b.type == "NUM"){

        int result = std::stoll(a.name) * std::stoll(b.name);
        setRegister("B",result);
    }

    if(a.type == "IDE" && b.type == "NUM"){

        loadFromMemory(a.name,"B");
        pushCommand("COPY C B");
        for(int i = 0; i < std::stoll(b.name) - 1; i++){
            pushCommand("ADD B C");
        }
    }

    if(a.type == "NUM" && b.type == "IDE"){

        loadFromMemory(b.name,"B");
        pushCommand("COPY C B");
        for(int i = 0; i < std::stoll(a.name) - 1; i++){
            pushCommand("ADD B C");
        }
    }
    if(a.type == "NUM" && b.type == "ARR"){
        loadValueFromTable(b.name);
        setRegister("C", stoll(a.name));
        multpPush();
    }

    if(a.type == "IDE" && b.type == "ARR"){
        loadValueFromTable(b.name);
        loadFromMemory(a.name, "C");
        multpPush();
    }
    if(a.type == "ARR" && b.type == "NUM"){
        loadValueFromTable(a.name);
        setRegister("C", stoll(b.name));
        multpPush();
    }
    if(a.type == "ARR" && b.type == "IDE"){
        loadValueFromTable(a.name);
        loadFromMemory(b.name,"C");
        multpPush();
    }
    if(a.type == "ARR" && b.type == "ARR"){
        loadValueFromTable(b.name);
        pushCommand("COPY E B");
        loadValueFromTable(a.name);
        pushCommand("COPY C E");
        
        multpPush();
    }
}

void divideO(Identifier a, Identifier b){

    if ((b.type == "IDE" && a.type == "IDE"))
    {
        loadFromMemory(a.name,"C");
        loadFromMemory(b.name,"D");
        dividePush();
      }

    if ((a.type == "NUM" && b.type == "NUM")){
        int result;
        if(std::stoll(b.name) != 0)
            result = std::stoll(a.name) / std::stoll(b.name);
        else
            result = 0;
        setRegister("B", result);
    }

    if ((a.type == "IDE" && b.type == "NUM")){
        loadFromMemory(a.name,"C");
        setRegister("D",std::stoll(b.name));
        dividePush();
    }

    if ((b.type == "IDE" && a.type == "NUM")){
        loadFromMemory(b.name,"D");
        setRegister("C",std::stoll(a.name));
        dividePush();
    }
    if(a.type == "NUM" && b.type == "ARR"){
        loadValueFromTable(b.name);
        setRegister("C", stoll(a.name));
        pushCommand("COPY D B");
        dividePush();
    }
    
    if(a.type == "IDE" && b.type == "ARR"){
        loadValueFromTable(b.name);
        loadFromMemory(a.name, "C");
        pushCommand("COPY D B");
        dividePush();
    }
    if(a.type == "ARR" && b.type == "NUM"){
        loadValueFromTable(a.name);
        setRegister("D", stoll(b.name));
        pushCommand("COPY C B");
        dividePush();
    }
    if(a.type == "ARR" && b.type == "IDE"){
        loadValueFromTable(a.name);
        loadFromMemory(b.name,"D");
        pushCommand("COPY C B");
        dividePush();
    }
    if(a.type == "ARR" && b.type == "ARR"){
        // cerr<<"Robimy dzielenie !!!"<<endl;
        loadValueFromTable(a.name);
        pushCommand("COPY F B");
        loadValueFromTable(b.name);
        pushCommand("COPY D B");
        pushCommand("COPY C F");
        dividePush();
        // pushCommand("PUT B");
    }

}

void moduloO(Identifier a, Identifier b){

    if ((b.type == "IDE" && a.type == "IDE")){
        loadFromMemory(a.name,"C");
        loadFromMemory(b.name,"D");
        moduloPush();
      }

    if ((a.type == "NUM" && b.type == "NUM")){
        int result;
        if(std::stoll(b.name) != 0)
            result = std::stoll(a.name) % std::stoll(b.name);
        else
            result = 0;
        setRegister("B", result);
    }

    if ((a.type == "IDE" && b.type == "NUM")){
        loadFromMemory(a.name,"C");
        setRegister("D",std::stoll(b.name));
        moduloPush();
    }

    if ((b.type == "IDE" && a.type == "NUM")){
        loadFromMemory(b.name,"D");
        setRegister("C",std::stoll(a.name));
        moduloPush();
    }
    if(a.type == "NUM" && b.type == "ARR"){
        loadValueFromTable(b.name);
        setRegister("C", stoll(a.name));
        pushCommand("COPY D B");
        moduloPush();
    }
    
    if(a.type == "IDE" && b.type == "ARR"){
        loadValueFromTable(b.name);
        loadFromMemory(a.name, "C");
        pushCommand("COPY D B");
        moduloPush();
    }
    if(a.type == "ARR" && b.type == "NUM"){
        loadValueFromTable(a.name);
        setRegister("D", stoll(b.name));
        pushCommand("COPY C B");
        moduloPush();
    }
    if(a.type == "ARR" && b.type == "IDE"){
        loadValueFromTable(a.name);
        loadFromMemory(b.name,"D");
        pushCommand("COPY C B");
        moduloPush();
    }
    if(a.type == "ARR" && b.type == "ARR"){
        loadValueFromTable(a.name);
        pushCommand("COPY F B");
        loadValueFromTable(b.name);
        pushCommand("COPY D B");
        pushCommand("COPY C F");
        moduloPush();
    }

}

void multpPush(){

    pushCommand("SUB D D");
    string jumpPosition = to_string(programCounter + 9);
    string jumpPosition1 = to_string(programCounter);
    pushCommand("JZERO C "+ jumpPosition);
    string jumpPosition2 = to_string(programCounter + 4);
    pushCommand("JODD C "+ jumpPosition2);
    pushCommand("ADD B B");
    pushCommand("HALF C");
    pushCommand("JUMP "+ jumpPosition1);
    pushCommand("ADD D B");
    pushCommand("ADD B B");
    pushCommand("HALF C");
    pushCommand("JUMP "+ jumpPosition1);
    pushCommand("COPY B D");
}
void dividePush(){
    string jumpPosition = to_string(programCounter + 23);
    pushCommand("JZERO D "+ jumpPosition);
    pushCommand("COPY E D");
    pushCommand("COPY B E");
    pushCommand("SUB B C");
    jumpPosition = to_string(programCounter + 2);
    pushCommand("JZERO B "+ jumpPosition);
    jumpPosition = to_string(programCounter + 3);
    pushCommand("JUMP "+ jumpPosition);
    pushCommand("ADD E E");
    string jzeroPosition = to_string(programCounter - 5);
    pushCommand("JUMP "+ jzeroPosition);
    pushCommand("SUB B B");
    pushCommand("COPY F E");
    pushCommand("SUB F C");
    jumpPosition = to_string(programCounter + 4);
    pushCommand("JZERO F "+ jumpPosition);
    pushCommand("ADD B B");
    pushCommand("HALF E");
    jzeroPosition = to_string(programCounter + 5);
    pushCommand("JUMP "+ jzeroPosition);
    pushCommand("ADD B B");
    pushCommand("INC B");
    pushCommand("SUB C E");
    pushCommand("HALF E");
    pushCommand("COPY F D");
    pushCommand("SUB F E");
    jumpPosition = to_string(programCounter - 12);
    pushCommand("JZERO F "+ jumpPosition);
    jzeroPosition = to_string(programCounter + 3);
    pushCommand("JUMP "+ jzeroPosition);
    pushCommand("SUB C C");
    pushCommand("COPY B C");
}

void moduloPush(){
        pushCommand("COPY G C");
        dividePush();
        pushCommand("COPY C D");
        pushCommand("COPY E D");
        multpPush();
        string jumpPos = to_string(programCounter + 4);
        pushCommand("JZERO E "+ jumpPos);
        pushCommand("SUB G B");
        pushCommand("COPY B G");
        string jumpPosition = to_string(programCounter + 2);
        pushCommand("JUMP "+jumpPosition);
        pushCommand("SUB B B");
}

void pushCommand(string command)
{
    codeStack.push_back(command);
    programCounter++;
}

void printCodeStd(){
    long long int i;
    for (i = 0; i < codeStack.size(); i++){
        
            cout << codeStack.at(i) << endl;   
    }       
}



////////////////////////////////////////////
// conditions functions
////////////////////////////////////////////
//TODO ARR & ARR
void setRegistersFromConditions(Identifier a, Identifier b, int yylineno){

    if ((a.type == "IDE" && b.type == "IDE")){
        loadFromMemory(a.name,"C");
        loadFromMemory(b.name,"D");
    }
    if(a.type == "NUM" && b.type == "NUM"){
        setRegister("C", stoll(a.name));
        setRegister("D", stoll(b.name));
    }
    if(a.type == "NUM" && b.type == "IDE"){
        setRegister("C", stoll(a.name));
        loadFromMemory(b.name,"D");
    }
    if(a.type == "IDE" && b.type == "NUM"){
        loadFromMemory(a.name, "C");
        setRegister("D", stoll(b.name));
    }
    if(a.type == "NUM" && b.type == "ARR"){
        
        string lastS = getIdeFromBrackets(b.name,'|');

        makeShiftOnTable(lastS, b.name);
        pushCommand("LOAD B");
        pushCommand("COPY D B");
        setRegister("C", stoll(a.name));
    }
    if(a.type == "IDE" && b.type == "ARR"){
        string lastS = getIdeFromBrackets(b.name,'|');

        makeShiftOnTable(lastS, b.name);
        pushCommand("LOAD B");
        pushCommand("COPY D B");
        loadFromMemory(a.name, "C");
        

    }
    if(a.type == "ARR" && b.type == "NUM"){
        string lastS = getIdeFromBrackets(b.name,'|');

        makeShiftOnTable(lastS, a.name);
        pushCommand("LOAD B");
        pushCommand("COPY C B");
        setRegister("D", stoll(b.name));
    }
    if(a.type == "ARR" && b.type == "IDE"){
        string lastS = getIdeFromBrackets(a.name,'|');
        makeShiftOnTable(lastS, a.name);
        pushCommand("LOAD B");
        pushCommand("COPY C B");
        loadFromMemory(b.name,"D");
    }
}

void equalCondition(string ide1, string ide2, int yylineno){

    Identifier a = ideStack.top();
    ideStack.pop();
    Identifier b = ideStack.top();
    ideStack.pop();

    setRegistersFromConditions(a, b, yylineno);

    pushCommand("COPY E D");
    pushCommand("SUB D C");
    string jumpPosition1 = to_string(programCounter + 2);
    pushCommand("JZERO D " + jumpPosition1);
    string jumpToJump = to_string(programCounter + 3);
    pushCommand("JUMP " + jumpToJump);
    pushCommand("SUB C E");
    string jumpPosition2 = to_string(programCounter + 2);
    pushCommand("JZERO C " + jumpPosition2);
    pushCommand("JUMP $mark");

}

void notEqualCondition(string ide1, string ide2, int yylineno){

    Identifier a = ideStack.top();
    ideStack.pop();
    Identifier b = ideStack.top();
    ideStack.pop();

    setRegistersFromConditions(a, b, yylineno);
    pushCommand("COPY E D");
    pushCommand("SUB D C");
    string jumpPosition1 = to_string(programCounter + 2);
    pushCommand("JZERO D " + jumpPosition1);
    string jumpToIf = to_string(programCounter + 5);
    pushCommand("JUMP " + jumpToIf);
    pushCommand("SUB C E");
    string jumpPosition2 = to_string(programCounter + 2);
    pushCommand("JZERO C " + jumpPosition2);
    string jumpPosition3 = to_string(programCounter + 2);
    pushCommand("JUMP "+jumpPosition3);
    pushCommand("JUMP $mark");

}

void greaterCondition(string ide1, string ide2, int yylineno){

    Identifier a = ideStack.top();
    ideStack.pop();
    Identifier b = ideStack.top();
    ideStack.pop();

    setRegistersFromConditions(a, b, yylineno);

    pushCommand("SUB D C");
    string jzeroPosition = to_string(programCounter + 2);
    pushCommand("JZERO D "+jzeroPosition);
    string jumpPostion = to_string(programCounter + 2);
    pushCommand("JUMP "+jumpPostion);
    pushCommand("JUMP $mark");
}

void lowerCondition(string ide1, string ide2, int yylineno){

    Identifier a = ideStack.top();
    ideStack.pop();
    Identifier b = ideStack.top();
    ideStack.pop();

    setRegistersFromConditions(a, b, yylineno);

    pushCommand("SUB C D");
    string jzeroPosition = to_string(programCounter + 2);
    pushCommand("JZERO C "+jzeroPosition);
    string jumpPostion = to_string(programCounter + 2);
    pushCommand("JUMP "+jumpPostion);
    pushCommand("JUMP $mark");
}

void greaterEqualCondition(string ide1, string ide2, int yylineno){

    Identifier a = ideStack.top();
    ideStack.pop();
    Identifier b = ideStack.top();
    ideStack.pop();

    setRegistersFromConditions(a, b, yylineno);

    pushCommand("COPY E D");

    pushCommand("SUB D C");
    string jumpPosition1 = to_string(programCounter + 2);
    pushCommand("JZERO D " + jumpPosition1);
    string jumpToJump = to_string(programCounter + 4);
    pushCommand("JUMP " + jumpToJump);
    pushCommand("SUB C E");
    string jumpPosition2 = to_string(programCounter + 2);
    pushCommand("JZERO C " + jumpPosition2);
    pushCommand("JUMP $mark");
}

void lowerEqualCondition(string ide1, string ide2, int yylineno){
    
    Identifier a = ideStack.top();
    ideStack.pop();
    Identifier b = ideStack.top();
    ideStack.pop();

    setRegistersFromConditions(a, b, yylineno);

    pushCommand("COPY E D");
    pushCommand("COPY F C");
    pushCommand("SUB F E");
    string jumpPosition0 = to_string(programCounter + 2);
    pushCommand("JZERO F "+jumpPosition0);
    string jumpPositionX = to_string(programCounter + 8);
    pushCommand("JUMP "+jumpPositionX);
    pushCommand("COPY E D");

    pushCommand("SUB D C");
    string jumpPosition1 = to_string(programCounter + 2);
    pushCommand("JZERO D " + jumpPosition1);
    string jumpToJump = to_string(programCounter + 3);
    pushCommand("JUMP " + jumpToJump);
    pushCommand("SUB C E");
    string jumpPosition2 = to_string(programCounter + 2);
    pushCommand("JZERO C " + jumpPosition2);
    pushCommand("JUMP $mark");
}


/////////////////////////////////////////////
// loop expression functions
////////////////////////////////////////////


void getRidOfUselessIterator(){
    string iteratorToDel = unChangeableIden.back();
    for(int i = 0; i < memoryTable.size(); i++){
        if(memoryTable[i] == iteratorToDel){
            memoryTable[i] = "none";
        }
    }
    unChangeableIden.pop_back();
}
void forArrCommandsBegin(Identifier startPoint){

        string lastS = getIdeFromBrackets(startPoint.name,'|');
        
        if(is_number(lastS)){
            loadFromMemory(startPoint.name,"G");
        }
        else{
            makeShiftOnTable(lastS, startPoint.name);
            pushCommand("LOAD B");
            pushCommand("COPY G B");
        }
}
void forArrCommandsEnd(Identifier endPoint){
    if(endPoint.type == "IDE"){
        loadFromMemory(endPoint.name,"H");
    }
    if(endPoint.type == "ARR"){
            string lastS = getIdeFromBrackets(endPoint.name,'|');
            if(is_number(lastS)){
                loadFromMemory(endPoint.name,"H");
            }
            else{
                makeShiftOnTable(lastS, endPoint.name);
                pushCommand("LOAD B");
                pushCommand("COPY H B");
            }
    }
}
void customForDeclaration(string ide, int yylineno){
    forCounter++;
    setVariables.push_back(ide);
    Identifier endPoint = ideStack.top();
    ideStack.pop();  
    Identifier startPoint = ideStack.top();
    ideStack.pop();
    declarationIde(ide, yylineno);

    endPointStack.push(endPoint);
    unChangeableIden.push_back(ide);
    pushCommand("SUB G G");
    if(startPoint.type == "NUM"){
        setRegister("G",stoll(startPoint.name));    
    }
    if(startPoint.type == "IDE"){
        
        loadFromMemory(startPoint.name,"G"); 
    }
    if(endPoint.type == "IDE"){
        forArrCommandsEnd(endPoint);
    }
    
    if(startPoint.type == "ARR"){
        forArrCommandsBegin(startPoint); 
    }
    if(endPoint.type == "ARR"){  
        forArrCommandsEnd(endPoint);
    }

    jumpStack.push(programCounter);
    storeInMemory("G", ide); 
    
}

void customFor(string iterator, string endpoint){

    loadFromMemory(iterator,"G");
    pushCommand("INC G");
    pushCommand("COPY D G");

    Identifier endPoint = endPointStack.top();
    endPointStack.pop();
    if(endPoint.type == "ARR"){
        pushCommand("COPY C H");
    }
    else{
     if(is_number(endpoint)){
        setRegister("C",stoll(endpoint));
    }
    else{
        if(endPointStack.empty() && forCounter == 1){
            // cerr<<"END POINT STACK EMPTY"<<endl;
            pushCommand("COPY C H");
            // loadFromMemory(endpoint,"C");
        }
        else{
            // cerr<<"END POINT STACK NOT EMPTY"<<endl;
            loadFromMemory(endpoint,"C");
        }
        //  pushCommand("PUT C");
        
    }
    }
    pushCommand("INC C");
    pushCommand("SUB C D");
    string jumpPosition = to_string(programCounter + 2);
    pushCommand("JZERO C " + jumpPosition);
    string beginPosition = to_string(jumpStack.top());
    jumpStack.pop();
    pushCommand("JUMP " + beginPosition);
    
    getRidOfUselessIterator();
}

void downtoForDeclaration(string ide, int yylineno){
    setVariables.push_back(ide);
    Identifier endPoint = ideStack.top();
    ideStack.pop();  
    Identifier startPoint = ideStack.top();
    ideStack.pop();
    declarationIde(ide, yylineno);
    
    endPointStack.push(endPoint);
    unChangeableIden.push_back(ide);
    pushCommand("SUB G G");
    if(startPoint.type == "NUM"){
        setRegister("G",stoll(startPoint.name));    
    }
    if(startPoint.type == "NUM"){
        setRegister("G",stoll(startPoint.name));
    }
    if(startPoint.type == "IDE"){
        loadFromMemory(startPoint.name,"G");
    }
    if(startPoint.type == "IDE"){
        loadFromMemory(startPoint.name,"G");
    }
    if(startPoint.type == "ARR"){
        forArrCommandsBegin(startPoint);  
        forArrCommandsEnd(endPoint);  
    }
    
    jumpStack.push(programCounter);
    storeInMemory("G", ide); 
}

void downtoFor(string iterator, string endpoint){

    loadFromMemory(iterator,"G");
    pushCommand("COPY D G");
    pushCommand("DEC G");

    Identifier endPoint = endPointStack.top();
    endPointStack.pop();
    if(endPoint.type == "ARR"){
        pushCommand("COPY C H");
    }
    
    else{
        if(is_number(endpoint)){
        setRegister("C",stoll(endpoint));
    }
    else{
        loadFromMemory(endpoint,"C");
    }
    }
    
    pushCommand("SUB D C");
    
    string jumpPosition = to_string(programCounter + 2);
    pushCommand("JZERO D " + jumpPosition);
    string beginPosition = to_string(jumpStack.top());
    jumpStack.pop();
    pushCommand("JUMP " + beginPosition);
    
    getRidOfUselessIterator();
}

void ifCondition() {
    //
}
void customIf() {

    jumpStackForLoop.push(programCounter);
    makeJump();

}

void elseInIf(){

    pushCommand("JUMP $elifmark");
    jumpStackForLoop.push(programCounter);
    makeJump();

}
void elseIf(){
    
    jumpStackForElIf.push(programCounter);
    makeElfJump();
    
}

void customWhileDeclaration(){
    
    jumpStack.push(programCounter);

}
void customWhile(){

    string beginPosition = to_string(jumpStack.top());
    jumpStack.pop();
    pushCommand("JUMP " + beginPosition);
    jumpStackForLoop.push(programCounter);
    makeJump();

}
void doWhileBegins(){

    jumpStack.push(programCounter);
    
}
void customDoWhile(){

    for(int i = codeStack.size() - 1; i >= 0; i--){
        if(codeStack.at(i) == "JUMP $mark"){
            string newJump = "JUMP "+to_string(programCounter+1);
            codeStack[i] = newJump;
            jumpStackForLoop.pop();
        }
    }
    string beginPosition = to_string(jumpStack.top());
    jumpStack.pop();
    pushCommand("JUMP " + beginPosition); 
}

///////////////////////////////////
// simple expression functions
//////////////////////////////////
void expressRead()
{
    Identifier a = ideStack.top();
    ideStack.pop();
    pushCommand("GET B");
    memoryTable.push_back(a.name);
    storeInMemory("B", a.name);
    setVariables.push_back(a.name);

}
void manageLongArray(string variable, string arrName){
    Identifier a;
    createIdentifier(&a,arrName,"IDE");
    memoryTable.push_back(arrName);
    loadFromMemory(variable,"B");
    storeInMemory("B", arrName);
    setVariables.push_back(a.name);
}
int findUndeclared(string var){
         for(int i = 0; i < setVariables.size(); i++){
             if(setVariables[i] == var)
                return 0;
         }
         return -1;
}
void ideAsignExpress(string ide, int yylineno)
{
    Identifier ideX [3];
    int i = 0;
    string ident = ide;
    while(!ideStack.empty()){
        ideX[i] = ideStack.top();
        ideStack.pop();
        i++;
    } 
    if(i == 1){
        setVariables.push_back(ideX[0].name);
        if(ideX[0].type == "ARR"){
            string lastS = getIdeFromBrackets(ideX[0].name,'|');

            if(!is_number(lastS)){
                makeShiftOnTable(lastS, ident);
                pushCommand("STORE B");
                return;
            }
            ide = ident + '|'+lastS; 
        }
    }
    else if(i == 2){
        if(findUndeclared(ideX[0].name) == -1 && ideX[0].type == "IDE"){
            cerr<<"ERROR: <line: "<<yylineno<<"> Próba użycia niezainicjalizowanej zmiennej: " <<ideX[0].name <<endl;
             exit(1);
        }
        setVariables.push_back(ideX[1].name);
        // tab(n) := tab(j) / tab(2) := x
        if(ideX[1].type == "ARR" && ideX[0].type == "ARR"){
            
            ident = ideX[0].name;
            string lastS = getIdeFromBrackets(ident,'|');
            if(!is_number(lastS)){
                makeShiftOnTable(lastS, ident);
                pushCommand("LOAD B");

                lastS = getIdeFromBrackets(ideX[1].name,'|');
                makeShiftOnTable(lastS, ideX[1].name);
                pushCommand("STORE B");
                return;
            }
            else{
                loadFromMemory(ideX[0].name,"B");
                storeInMemory("B",ideX[1].name);
            }
        }
        // tab(n) := x / tab(2) := x
        else if(ideX[1].type == "ARR"){

            string lastS = getIdeFromBrackets(ideX[1].name,'|');
            
            if(!is_number(lastS)){
                

                if(ideX[0].type == "IDE"){
                    if(longArr == 1){
                        manageLongArray(ideX[0].name, ideX[1].name);
                        return;
                    }
                    loadFromMemory(ideX[0].name,"B");
                }
                    
                if(ideX[0].type == "NUM")
                    setRegister("B", stoll(ideX[0].name));

                
                makeShiftOnTable(lastS, ident);
               
                pushCommand("STORE B");
                return;  
            }
            
            ide = ident + '|'+lastS;
        }
        else if(ideX[0].type == "ARR"){
            string lastS = getIdeFromBrackets(ideX[0].name,'|');
            // x := tab(n)
            if(!is_number(lastS)){ 
                makeShiftOnTable(lastS, ideX[0].name);
                pushCommand("LOAD B");   
            }
            // x := tab(3)
            else{
                loadFromMemory(ideX[0].name,"B");
            }
        }
        // m := n
        else if (ideX[1].type == "IDE" && ideX[0].type == "IDE"){
            
            loadFromMemory(ideX[0].name,"B");
        }
        // tab(x) := tab(y)
    
    }

    for(int i = 0; i < unChangeableIden.size(); i++){
        setVariables.push_back(unChangeableIden[i]);
        if(unChangeableIden[i] == ide){
            cerr<<"ERROR: <line: "<<yylineno<<"> Próba zmiany niezmiennej zmiennej."<<endl;
            exit(1);
        }
    }
    setVariables.push_back(ide);
    storeInMemory("B", ide);
}

void makeShiftOnTable(string lastS, string ide){
    if(is_number(lastS)){
        loadFromMemory(ide,"B");
        return;
    }
    loadFromMemory(lastS,"C");
    int tableBegins = findTableBeginning(getIdeFromArray(ide,'|'));
    string tableName = getIdeFromArray(ide,'|');
    int tableNameLen = tableName.length();

    setRegister("A", tableBegins);

    string tableBeginIndex = memoryTable[tableBegins];

    int tableBeginInd = stoll(getIdeFromBrackets(tableBeginIndex,'|'));
    setRegister("D", tableBeginInd);
    pushCommand("SUB C D");
    string jzeroPos = to_string(programCounter);
    string jumpPosition = to_string(programCounter + 4);
    pushCommand("JZERO C "+jumpPosition);
    pushCommand("INC A");
    pushCommand("DEC C");
    pushCommand("JUMP " + jzeroPos);
    
}
void expressWrite() {

    Identifier a = ideStack.top();
    ideStack.pop();
    if( !(find(setVariables.begin(), setVariables.end(), a.name) != setVariables.end()) && a.type == "IDE" ){
        cerr<<"ERROR: <line: "<<yylineno<<"> Próba użycia niezainicjalizowanej zmiennej."<<endl;
        exit(1);
    }
    if(a.type == "NUM"){
        setRegister("B", stoll(a.name));
        // do nothing
    }
    else if(a.type == "ARR"){
        if(longArr == 1){
            loadFromMemory(a.name,"B");
            longArr = 0;
            pushCommand("PUT B");
            return;
        }

        string ident = a.name;
        char last = ident.back();
        if(!isdigit(last)){
            string lastS ="";
            lastS.push_back(last);
            makeShiftOnTable(lastS, ident);
            pushCommand("LOAD B");
        }
        else{
            loadFromMemory(a.name,"B");
        }
    }
    else{
        loadFromMemory(a.name,"B");
    }
    pushCommand("PUT B");
}

void add(string ide1, string ide2, int yylineo){

    Identifier a = ideStack.top();
    ideStack.pop();
    Identifier b = ideStack.top();
    ideStack.pop();
    addO(a, b);
}
void sub(string ide1, string ide2, int yylineo){

    Identifier a = ideStack.top();
    ideStack.pop();
    Identifier b = ideStack.top();
    ideStack.pop();
    subO(b, a);

}
void multp(string ide1, string ide2, int yylineo){

    Identifier a = ideStack.top();
    ideStack.pop();
    Identifier b = ideStack.top();
    ideStack.pop();
    
    multpO(b,a);

}
void divide(string ide1, string ide2, int yylineo){
        Identifier a = ideStack.top();
        ideStack.pop();
        Identifier b = ideStack.top();
        ideStack.pop();
        divideO(b,a);
}
void modulo(string ide1, string ide2, int yylineo){
        Identifier a = ideStack.top();
        ideStack.pop();
        Identifier b = ideStack.top();
        ideStack.pop();
        moduloO(b,a);
}

void getNumber(string num){
    Identifier s;
    createIdentifier(&s, num, "NUM");
    ideStack.push(s);
    setRegister("B", stoll(num));
}
int checkIfArray(string ide){
    int probBeginIndex = findInVector(ide);
    string str1 = "|";
    if(memoryTable.size() > probBeginIndex + 1 && longArr == 0){
        size_t found = memoryTable[probBeginIndex + 1].find(str1); 
        if (found != string::npos) 
            return -1;
        else return 0;
    }
    else return 0;

}
void getIdentifier(string ide, int yylineno){

    Identifier s;
    createIdentifier(&s,ide, "IDE");
    ideStack.push(s);
    if(findInVector(ide) == -1){
        cerr << "ERROR: <line " << yylineno << "> Niezadeklarowana zmienna: " << ide << endl;
        exit(1);
    }
    //HERE IS MY NEW
    if(checkIfArray(ide) == -1){
        cerr << "ERROR: <line " << yylineno << "> Złe użycie zmiennej tablicowej: " << ide << endl;
        exit(1);
    }
}

void getArrayWithNum(string ide, string place){

    Identifier s;
    string name = ide +'|'+ place;
    createIdentifier(&s,name, "ARR");
    ideStack.push(s);
}
void getArrayWithIde(string ide, string place){
    
    Identifier s;
    string name = ide +'|'+ place;
    createIdentifier(&s,name, "ARR");
    ideStack.push(s);
}

void declarationIde(string ide, int yylineno){
    if ( std::find(memoryTable.begin(), memoryTable.end(), ide) != memoryTable.end() ){
        cerr << "ERROR: <line " << yylineno << "> Kolejna deklaracja zmiennej: " << ide << endl;
        exit(1);
    }
    else{
        Identifier s;
        createIdentifier(&s, ide, "IDE");
        memoryTable.push_back(ide);
    }
}

void declarationArray(string ide, string num1, string num2, int yylineno){
    if ( std::find(memoryTable.begin(), memoryTable.end(), ide) != memoryTable.end() ){
        cerr << "ERROR: <line" << yylineno << "> Kolejna deklaracja zmiennej: " << ide << endl;
        exit(1);
    }
    else if((stoll(num2) - stoll(num1)) < 0) {
        cerr << "ERROR: <line" << yylineno << "> Zły zakres deklaracji tablicy: " << ide << endl;
        exit(1);
    }
    else{
        Identifier s;
        createIdentifier(&s, ide, "IDE");
        memoryTable.push_back(ide);
        if(stoll(num2)- stoll(num1) > 40000000){
            LongArray longArray;
            createLongArray(&longArray,stoll(num1),stoll(num2));
            longArr = 1;
            return;
        }
        for(long long int i = stoll(num1); i <= stoll(num2); i++){
            
            string myIde = ide+'|'+to_string(i);
            memoryTable.push_back(myIde);
        }      
    }
}

//////////////////////////////////
// memory managment functions
/////////////////////////////////

string DecToBin(long long int number)
{
    if ( number == 0 ) return "0";
    if ( number == 1 ) return "1";

    if ( number % 2 == 0 )
        return DecToBin(number / 2) + "0";
    else
        return DecToBin(number / 2) + "1";
}

void setRegister(string reg, long long int value){
    pushCommand("SUB "+reg+" "+reg);
    if(value < 8){
        for(int i = 0; i < value; i++){
            pushCommand("INC "+reg);
        }
        return;
    }
    
    string regValueBin = DecToBin(value);
    long long int end = regValueBin.size();
    
    for(long long int i = 0; i < end; i++){
        if(regValueBin[i] == '1'){
            pushCommand("INC "+ reg);
        }
        if(i < (end - 1)){
            pushCommand("ADD "+reg+" "+reg);
        }
    }
}

void storeInMemory(string reg, string variable){

    int placeInMemory = findInVector(variable);

    if(placeInMemory == -1)
        setRegister("A", (long long)(memoryTable.size()-1));
    else
        setRegister("A",placeInMemory);

    pushCommand("STORE "+reg);
}

void loadFromMemory(string variable, string reg){

    int placeInMemory = findInVector(variable);
    if(placeInMemory == -1){
        cerr<<" ERROR"<<" Próba użycia niezadeklarowanej zmiennej "<<variable<<endl;
        exit(1);
    }
    setRegister("A", placeInMemory);
    pushCommand("LOAD "+reg);
}

int findInVector(string var){

    for(int i = 0; i < memoryTable.size(); i++){
        if(memoryTable[i] == var){
            return i;
        }
    }
    return -1;
}

int findTableBeginning(string var){
    for(int i = 0; i < memoryTable.size(); i++){
        if(memoryTable[i] == var){
            return i + 1;
        }
    }
    return -1;
}


