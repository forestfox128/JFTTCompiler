#include "compiler.hpp"

//map<string, Identifier> identifierStack;
map<string, string> registerMap;
stack<Identifier> ideStack;
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
        // cerr<<i<<" "<< codeStack.at(i)<<endl;
        // cerr<<jumpStackForLoop.top()<<endl;
        if(codeStack.at(i) == "JUMP $mark"){
            string newJump = "JUMP "+ to_string(jumpStackForLoop.top());
            codeStack[i] = newJump;
            jumpStackForLoop.pop();
            return;
        }
        // else(codeStack.at(i) == "JUMP $elifmark"){
        //     //cout<<"tutaj ten jump"<<endl;
        //     cout<<"JUMP "<<jumpStackForElIf.top()<<endl;
        //     jumpStackForElIf.pop();
        // }
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

void addO(Identifier a, Identifier b)
{
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
}

void subO(Identifier a, Identifier b)
{
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
}

void multpO(Identifier a, Identifier b, Identifier c)
{

    if ((b.type == "IDE" && a.type == "IDE"))
    {
        loadFromMemory(a.name,"C");
        loadFromMemory(b.name,"B");
        pushCommand("COPY D B");
        string jzeroPosition = to_string(programCounter);
        string jumpPosition = to_string(programCounter + 4);
        pushCommand("JZERO C "+ jumpPosition);
        pushCommand("ADD B D");
        pushCommand("DEC C");
        pushCommand("JUMP "+ jzeroPosition);
        pushCommand("SUB B D");
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
}

void divideO(Identifier a, Identifier b, Identifier c)
{

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

}

void moduloO(Identifier a, Identifier b)
{

    if ((b.type == "IDE" && a.type == "IDE"))
    {
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

}

void dividePush(){
        pushCommand("SUB B B");
        pushCommand("COPY E C");
        string jzeroPosition = to_string(programCounter);
        pushCommand("SUB E D");
        string jumpPosition1 = to_string(programCounter + 6);
        pushCommand("JZERO E "+jumpPosition1);
        string jumpPosition = to_string(programCounter + 8);
        pushCommand("JZERO D "+jumpPosition);
        string jumpPosition2 = to_string(programCounter);
        pushCommand("JZERO C "+jumpPosition);
        pushCommand("SUB C D");
        pushCommand("INC B");
        pushCommand("JUMP "+jzeroPosition);
        pushCommand("COPY E D");
        pushCommand("SUB E C");
        pushCommand("JZERO E "+jumpPosition2);
}

void moduloPush(){
        pushCommand("SUB B B");
        pushCommand("COPY E C");
        pushCommand("COPY F D");
        pushCommand("SUB E D");
        string jumpPosition1 = to_string(programCounter + 6);
        pushCommand("JZERO E "+jumpPosition1);
        string jumpPosition = to_string(programCounter + 13);
        pushCommand("JZERO D "+jumpPosition);
        string jzeroPosition = to_string(programCounter);
        string jzeroPosition1 = to_string(programCounter + 1);
        string jumpPosition2 = to_string(programCounter + 8);
        pushCommand("JZERO C "+jumpPosition2);
        pushCommand("COPY B C");
        pushCommand("SUB C D");
        pushCommand("JUMP "+jzeroPosition);
        pushCommand("COPY E D");
        pushCommand("SUB E C");
        pushCommand("JZERO E "+jumpPosition);
        pushCommand("JUMP "+jzeroPosition1);
        pushCommand("SUB F B");
        string jumpPosition3 = to_string(programCounter + 2);
        pushCommand("JZERO F "+jumpPosition3);
        pushCommand("JUMP "+jumpPosition);
        pushCommand("SUB B B");
}

void pushCommand(string command)
{
    codeStack.push_back(command);
    programCounter++;
}

void printCodeStd()
{
    long long int i;
    for (i = 0; i < codeStack.size(); i++){
        
            cout << codeStack.at(i) << endl;
        
    }
        
}

////////////////////////////////////////////
// conditions functions
////////////////////////////////////////////
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

void customForDeclaration(string ide, int yylineno){

    Identifier endPoint = ideStack.top();
    ideStack.pop();  
    Identifier startPoint = ideStack.top();
    ideStack.pop();
    declarationIde(ide, yylineno);
    
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
    
    jumpStack.push(programCounter);
    storeInMemory("G", ide); 
    
}

void customFor(string iterator, string endpoint){

    loadFromMemory(iterator,"G");
    pushCommand("INC G");
    pushCommand("COPY D G");
    if(is_number(endpoint)){
        setRegister("C",stoi(endpoint));
    }
    else{
        //cout<<endpoint<<endl;
        loadFromMemory(endpoint,"C");
    }
    pushCommand("INC C");
    pushCommand("SUB C D");
    string jumpPosition = to_string(programCounter + 2);
    pushCommand("JZERO C " + jumpPosition);
    string beginPosition = to_string(jumpStack.top());
    jumpStack.pop();
    pushCommand("JUMP " + beginPosition);
    
    
}


void downtoForDeclaration(string ide, int yylineno){

    Identifier endPoint = ideStack.top();
    ideStack.pop();  
    Identifier startPoint = ideStack.top();
    ideStack.pop();
    declarationIde(ide, yylineno);
    
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
    
    jumpStack.push(programCounter);
    storeInMemory("G", ide); 
}


void downtoFor(string iterator, string endpoint){

    loadFromMemory(iterator,"G");
    pushCommand("COPY D G");
    pushCommand("DEC G");
    if(is_number(endpoint)){
        setRegister("C",stoi(endpoint));
    }
    else{
        loadFromMemory(endpoint,"C");
    }
    pushCommand("SUB D C");
    string jumpPosition = to_string(programCounter + 2);
    pushCommand("JZERO D " + jumpPosition);
    string beginPosition = to_string(jumpStack.top());
    jumpStack.pop();
    pushCommand("JUMP " + beginPosition);
}

void ifCondition() {
    //
}
void customIf() {

    jumpStackForLoop.push(programCounter);
    makeJump();

}

void elseInIf(){

    // pushCommand("JUMP $elifmark");
    // jumpStackForLoop.push(programCounter);
    // makeJump();

}
void elseIf(){
    
    // jumpStackForElIf.push(programCounter);
    
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
void customDoWhile(){

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

void ideAsignExpress(string ide, int yylineno)
{
    for(int i = 0; i < unChangeableIden.size(); i++){
        if(unChangeableIden[i] == ide){
            cerr<<"ERROR: <line: "<<yylineno<<"> Próba zmiany niezmiennej zmiennej."<<endl;
            exit(1);
        }
    }
    storeInMemory("B", ide);

}
void expressWrite() {

    Identifier a = ideStack.top();
    ideStack.pop();
    if(a.type == "NUM"){
        setRegister("B", stoi(a.name));
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
    Identifier c = ideStack.top();
    ideStack.pop();
    multpO(b,a,c);

}
void divide(string ide1, string ide2, int yylineo){
        Identifier a = ideStack.top();
        ideStack.pop();
        Identifier b = ideStack.top();
        ideStack.pop();
        Identifier c = ideStack.top();
        ideStack.pop();
        divideO(b,a,c);
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
    setRegister("B", std::stoi(num));
}

void getIdentifier(string ide){

    Identifier s;
    createIdentifier(&s,ide, "IDE");
    ideStack.push(s);
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

//////////////////////////////////
// memory managment functions
/////////////////////////////////
void setRegister(string reg, int value){
    pushCommand("SUB "+reg+" "+reg);
     
    for(int i = 0; i < value; i++){
        pushCommand("INC "+reg);
    }
}

void storeInMemory(string reg, string variable){

    int placeInMemory = findInVector(variable);

    if(placeInMemory == -1)
        setRegister("A", memoryTable.size()-1);
    else
        setRegister("A",placeInMemory);

    pushCommand("STORE "+reg);
}

void loadFromMemory(string variable, string reg){

    int placeInMemory = findInVector(variable);
    if(placeInMemory == -1){
        cout<<" ERROR"<<programCounter<<" "<<variable<<endl;
        return;
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


