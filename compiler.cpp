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
queue <string> tempCodeQueue;

long long int programCounter = 0;
long long int tempProgramCounter = 0;

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
void createIdentifier(Identifier *s, string name, string type)
{
    s->name = name;
    s->type = type;
}

void loadValueFromTable(string ide){
    char last = ide.back();
    if(!isdigit(last)){
        string lastS ="";
        lastS.push_back(last);
        makeShiftOnTable(lastS, ide);
        pushCommand("LOAD B");
    }
    else{
        loadFromMemory(ide,"B");
    }
}
void addO(Identifier a, Identifier b){
    if ((a.type == "IDE" && b.type == "NUM"))
    {
        loadFromMemory(a.name,"B");
        for (int i = 0; i < std::stoi(b.name); i++)
        {
            pushCommand("INC B");
        }
    }
    if (a.type == "NUM" && b.type == "IDE")
    {
        loadFromMemory(b.name,"B");
        for (int i = 0; i < std::stoi(a.name); i++)
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

        setRegister("B",std::stoi(a.name));
        setRegister("C",std::stoi(b.name));
        pushCommand("ADD B C");
    }
    if(a.type == "NUM" && b.type == "ARR"){
        loadValueFromTable(b.name);
        setRegister("C", stoi(a.name));
        pushCommand("ADD B C");
    }
    if(a.type == "IDE" && b.type == "ARR"){
        loadValueFromTable(b.name);
        loadFromMemory(a.name, "C");
        pushCommand("ADD B C");
    }
    if(a.type == "ARR" && b.type == "NUM"){
        loadValueFromTable(a.name);
        setRegister("C", stoi(b.name));
        pushCommand("ADD B C");
    }
    if(a.type == "ARR" && b.type == "IDE"){
        loadValueFromTable(a.name);
        loadFromMemory(b.name,"C");
        pushCommand("ADD B C");
    }
    if(a.type == "ARR" && b.type == "ARR"){
        loadValueFromTable(a.name);
        pushCommand("COPY C B");
        loadValueFromTable(b.name);
        pushCommand("ADD B C");
    }
}

void subO(Identifier a, Identifier b){
    if ((a.type == "IDE" && b.type == "NUM"))
    {
        loadFromMemory(a.name,"B");
        for (int i = 0; i < std::stoi(b.name); i++)
        {
            pushCommand("DEC B");
        }
    }
    if (a.type == "NUM" && b.type == "IDE")
    {
        loadFromMemory(b.name,"C");
        setRegister("B",std::stoi(a.name));
        pushCommand("SUB B C");
    }
    if (a.type == "IDE" && b.type == "IDE")
    {
        loadFromMemory(a.name,"B");
        loadFromMemory(b.name,"C");
        pushCommand("SUB B C");
    }
    if(a.type == "NUM" && b.type == "NUM"){

        setRegister("B",std::stoi(a.name));
        setRegister("C",std::stoi(b.name));
        pushCommand("SUB B C");
    }
    if(a.type == "NUM" && b.type == "ARR"){
        loadValueFromTable(b.name);
        setRegister("C", stoi(a.name));
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
        setRegister("C", stoi(b.name));
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

        int result = std::stoi(a.name) * std::stoi(b.name);
        setRegister("B",result);
    }

    if(a.type == "IDE" && b.type == "NUM"){

        loadFromMemory(a.name,"B");
        pushCommand("COPY C B");
        for(int i = 0; i < std::stoi(b.name) - 1; i++){
            pushCommand("ADD B C");
        }
    }

    if(a.type == "NUM" && b.type == "IDE"){

        loadFromMemory(b.name,"B");
        pushCommand("COPY C B");
        for(int i = 0; i < std::stoi(a.name) - 1; i++){
            pushCommand("ADD B C");
        }
    }
    if(a.type == "NUM" && b.type == "ARR"){
        loadValueFromTable(b.name);
        setRegister("C", stoi(a.name));
        multpPush();
    }

    if(a.type == "IDE" && b.type == "ARR"){
        loadValueFromTable(b.name);
        loadFromMemory(a.name, "C");
        multpPush();
    }
    if(a.type == "ARR" && b.type == "NUM"){
        loadValueFromTable(a.name);
        setRegister("C", stoi(b.name));
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
        if(std::stoi(b.name) != 0)
            result = std::stoi(a.name) / std::stoi(b.name);
        else
            result = 0;
        setRegister("B", result);
    }

    if ((a.type == "IDE" && b.type == "NUM")){
        loadFromMemory(a.name,"C");
        setRegister("D",std::stoi(b.name));
        dividePush();
    }

    if ((b.type == "IDE" && a.type == "NUM")){
        loadFromMemory(b.name,"D");
        setRegister("C",std::stoi(a.name));
        dividePush();
    }
    if(a.type == "NUM" && b.type == "ARR"){
        loadValueFromTable(b.name);
        setRegister("C", stoi(a.name));
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
        setRegister("D", stoi(b.name));
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
        pushCommand("COPY C B");
        loadValueFromTable(b.name);
        pushCommand("COPY D B");
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
        if(std::stoi(b.name) != 0)
            result = std::stoi(a.name) % std::stoi(b.name);
        else
            result = 0;
        setRegister("B", result);
    }

    if ((a.type == "IDE" && b.type == "NUM")){
        loadFromMemory(a.name,"C");
        setRegister("D",std::stoi(b.name));
        moduloPush();
    }

    if ((b.type == "IDE" && a.type == "NUM")){
        loadFromMemory(b.name,"D");
        setRegister("C",std::stoi(a.name));
        moduloPush();
    }
    if(a.type == "NUM" && b.type == "ARR"){
        loadValueFromTable(b.name);
        setRegister("C", stoi(a.name));
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
        setRegister("D", stoi(b.name));
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
        pushCommand("COPY C B");
        loadValueFromTable(b.name);
        pushCommand("COPY D B");
        moduloPush();
    }

}

void multpPush(){
    pushCommand("COPY D B");
    pushCommand("SUB B B");
    string jzeroPosition = to_string(programCounter);
    string jumpPosition = to_string(programCounter + 7);
    pushCommand("JZERO C "+ jumpPosition);
    string jzeroPosition1 = to_string(programCounter + 2);
    string jzeroPosition2 = to_string(programCounter + 3);
    pushCommand("JODD D "+ jzeroPosition1);
    pushCommand("JUMP "+ jzeroPosition2);
    pushCommand("ADD B C");
    pushCommand("ADD C C");
    pushCommand("HALF D");
    pushCommand("JUMP "+ jzeroPosition);
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
//MODULO sub B B
void moduloPush(){
        pushCommand("COPY G C");
        pushCommand("COPY H D");
        dividePush();
        pushCommand("COPY C H");
        multpPush();
        pushCommand("SUB G B");
        pushCommand("COPY B G");
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
        setRegister("C", stoi(a.name));
        setRegister("D", stoi(b.name));
    }
    if(a.type == "NUM" && b.type == "IDE"){
        setRegister("C", stoi(a.name));
        loadFromMemory(b.name,"D");
    }
    if(a.type == "IDE" && b.type == "NUM"){
        loadFromMemory(a.name, "C");
        setRegister("D", stoi(b.name));
    }
    if(a.type == "NUM" && b.type == "ARR"){
        
        char last = b.name.back();
        string lastS ="";
        lastS.push_back(last);
        makeShiftOnTable(lastS, b.name);
        pushCommand("LOAD B");
        pushCommand("COPY D B");
        setRegister("C", stoi(a.name));
    }
    if(a.type == "IDE" && b.type == "ARR"){
        // cerr<<"HERE@@@@@"<<endl;
        char last = b.name.back();
        string lastS ="";
        lastS.push_back(last);
        makeShiftOnTable(lastS, b.name);
        pushCommand("LOAD B");
        pushCommand("COPY D B");
        loadFromMemory(a.name, "C");

    }
    if(a.type == "ARR" && b.type == "NUM"){
        char last = a.name.back();
        string lastS ="";
        lastS.push_back(last);
        makeShiftOnTable(lastS, a.name);
        pushCommand("LOAD B");
        pushCommand("COPY C B");
        setRegister("D", stoi(b.name));
    }
    if(a.type == "ARR" && b.type == "IDE"){
        char last = a.name.back();
        string lastS ="";
        lastS.push_back(last);
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

bool is_number(const std::string& s){

    return !s.empty() && std::find_if(s.begin(), 
        s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
}
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
    char last = startPoint.name.back();
        string lastS ="";
        lastS.push_back(last);
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
            char last = endPoint.name.back();
            string lastS ="";
            lastS.push_back(last);
            if(is_number(lastS)){
                loadFromMemory(endPoint.name,"H");
            }
            else{
                makeShiftOnTable(lastS, endPoint.name);
                pushCommand("LOAD B");
                pushCommand("COPY H B");
            }
}
void customForDeclaration(string ide, int yylineno){

    Identifier endPoint = ideStack.top();
    ideStack.pop();  
    Identifier startPoint = ideStack.top();
    ideStack.pop();
    declarationIde(ide, yylineno);

    endPointStack.push(endPoint);
    unChangeableIden.push_back(ide);
    pushCommand("SUB G G");
    if(startPoint.type == "NUM"){
        setRegister("G",stoi(startPoint.name));    
    }
    if(startPoint.type == "NUM"){
        setRegister("G",stoi(startPoint.name));
    }
    if(startPoint.type == "IDE"){
        loadFromMemory(startPoint.name,"G");
    }
    if(startPoint.type == "IDE"){
        loadFromMemory(startPoint.name,"G");
    }
    if(startPoint.type == "ARR"){
        // cerr<<"Starting as an Array "<<startPoint.name<<endl;
        forArrCommandsBegin(startPoint);   
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
        setRegister("C",stoi(endpoint));
    }
    else{
        //cout<<endpoint<<endl;
        loadFromMemory(endpoint,"C");
    }
    }
    // pushCommand("PUT C");
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

    Identifier endPoint = ideStack.top();
    ideStack.pop();  
    Identifier startPoint = ideStack.top();
    ideStack.pop();
    declarationIde(ide, yylineno);
    
    endPointStack.push(endPoint);
    unChangeableIden.push_back(ide);
    pushCommand("SUB G G");
    if(startPoint.type == "NUM"){
        setRegister("G",stoi(startPoint.name));    
    }
    if(startPoint.type == "NUM"){
        setRegister("G",stoi(startPoint.name));
    }
    if(startPoint.type == "IDE"){
        loadFromMemory(startPoint.name,"G");
    }
    if(startPoint.type == "IDE"){
        loadFromMemory(startPoint.name,"G");
    }
    if(startPoint.type == "ARR"){
        // cerr<<"Starting as an Array "<<startPoint.name<<endl;
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
        setRegister("C",stoi(endpoint));
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

}
//potrzebuję m:=n;
void ideAsignExpress(string ide, int yylineno)
{
    Identifier ideX [3];
    int i = 0;
    string ident = ide;
    while(!ideStack.empty()){
        ideX[i] = ideStack.top();
        // cerr<<"------> "<< ideX[i].name<<endl;
        ideStack.pop();
        i++;
    } 
    if(i == 1){
        // tab(n) := x+x / tab(3) := x+x
        // cerr<<"tab(n) := x+x "<<ideX[0].name<<ideX[0].type<<endl;
        if(ideX[0].type == "ARR"){
            ident = ideX[0].name;
            char last = ident.back();
            
            if(!isdigit(last)){
                string lastS ="";
                lastS.push_back(last);
                makeShiftOnTable(lastS, ident);
                // pushCommand("PUT B");
                pushCommand("STORE B");
                return;
            }
            ide = ident;  
        }
    }
    else if(i == 2){
        // tab(n) := tab(j) / tab(2) := x
        if(ideX[1].type == "ARR" && ideX[0].type == "ARR"){
            
            ident = ideX[0].name;
            char last = ident.back();
            if(!isdigit(last)){
                string lastS ="";
                lastS.push_back(last); 
                makeShiftOnTable(lastS, ident);
                pushCommand("LOAD B");

                ident = ideX[1].name;
                last = ident.back();
                lastS ="";
                lastS.push_back(last);
                makeShiftOnTable(lastS, ident);
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
            ident = ideX[1].name;
            char last = ident.back();
            if(!isdigit(last)){
                string lastS ="";
                lastS.push_back(last);

                if(ideX[0].type == "IDE")
                    loadFromMemory(ideX[0].name,"B");
                if(ideX[0].type == "NUM")
                    setRegister("B", stoi(ideX[0].name));
                // cerr<<"MAKE sHSIft "<<lastS<<" "<<ident<<endl;
                makeShiftOnTable(lastS, ident);
                // pushCommand("PUT B");
                pushCommand("STORE B");
                return;  
            }
            ide = ident;
        }
        else if(ideX[0].type == "ARR"){
            ident = ideX[0].name;
            char last = ident.back();
            // x := tab(n)
            if(!isdigit(last)){
                string lastS ="";
                lastS.push_back(last); 
                makeShiftOnTable(lastS, ident);
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
        if(unChangeableIden[i] == ide){
            cerr<<"ERROR: <line: "<<yylineno<<"> Próba zmiany niezmiennej zmiennej."<<endl;
            exit(1);
        }
    }
    
    storeInMemory("B", ide);
}

void makeShiftOnTable(string lastS, string ide){
    // cerr<<"WIG "<<lastS<<" "<<ide<<endl;
    if(is_number(lastS)){
        loadFromMemory(ide,"B");
        return;
    }
    loadFromMemory(lastS,"C");
    int tableBegins = findTableBeginning(ide.substr(0, ide.size()-1));
    string tableName = ide.substr(0, ide.size()-1);
    int tableNameLen = tableName.length();

    setRegister("A", tableBegins);

    string tableBeginIndex = memoryTable[tableBegins];
    // cerr<<"MAKE SHIFT "<<tableBeginIndex.substr(tableNameLen,tableBeginIndex.size())<<endl;
    int tableBeginInd = stoi(tableBeginIndex.substr(tableNameLen,tableBeginIndex.size()));
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
    if(a.type == "NUM"){
        setRegister("B", stoll(a.name));
        // do nothing
    }
    else if(a.type == "ARR"){
        //do nothing
        //loadFromMemory(a.name,"B");
        // cerr<<"A>NAME "<<a.name<<endl;
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

void getIdentifier(string ide){

    Identifier s;
    createIdentifier(&s,ide, "IDE");
    ideStack.push(s);
}

void getArrayWithNum(string ide, string place){

    Identifier s;
    string name = ide + place;
    createIdentifier(&s,name, "ARR");
    ideStack.push(s);
}
void getArrayWithIde(string ide, string place){
    
    Identifier s;
    string name = ide + place;
    createIdentifier(&s,name, "ARR");
    // cerr<<"REad arr variabkle: "<<name<<endl;
    ideStack.push(s);
    // char last = ide.back();
    // string lastS = place;
    // makeShiftOnTable(lastS, name);
    // pushCommand("LOAD B");
}

void declarationIde(string ide, int yylineno){
    if ( std::find(memoryTable.begin(), memoryTable.end(), ide) != memoryTable.end() ){
        cerr << "ERROR: <line" << yylineno << "> Kolejna deklaracja zmiennej: " << ide << endl;
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
    else if((stoi(num2) - stoi(num1)) <= 0) {
        cerr << "ERROR: <line" << yylineno << "> Zły zakres deklaracji tablicy: " << ide << endl;
        exit(1);
    }
    else{
        Identifier s;
        createIdentifier(&s, ide, "IDE");
        memoryTable.push_back(ide);
        for(int i = stoll(num1); i <= stoll(num2); i++){
            string myIde = ide+to_string(i);
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
        cerr<<" ERROR"<<programCounter<<" Próba użycia niezadeklarowanej zmiennej "<<variable<<endl;
        exit(1);
    }
    setRegister("A", placeInMemory);
    pushCommand("LOAD "+reg);
}

int findInVector(string var){

    for(int i = 0; i < memoryTable.size(); i++){
        //cerr<<i<<" "<<memoryTable[i]<<endl;
        if(memoryTable[i] == var){
            return i;
        }
    }
    return -1;
}

int findTableBeginning(string var){
    for(int i = 0; i < memoryTable.size(); i++){
        //cerr<<i<<" "<<memoryTable[i]<<endl;
        if(memoryTable[i] == var){
            return i + 1;
        }
    }
    return -1;
}


