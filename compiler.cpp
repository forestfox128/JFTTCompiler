#include "compiler.hpp"

//map<string, Identifier> identifierStack;
map<string, string> registerMap;
stack<Identifier> ideStack;
vector<string> codeStack;
stack<string> regStack;
vector <string> memoryTable;

int programCounter = 0;
string currentRegister = "";
int numberofVariables = 0;


void initializeStackReg()
{
    regStack.push("E");
    regStack.push("D");
    regStack.push("C");
    regStack.push("B");
}

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
    for (i = 0; i < codeStack.size(); i++)
        cout << codeStack.at(i) << endl;
}

////////////////////
void declarationIde(string ide, int yylineno)
{
    //if (identifierStack.find(ide) != identifierStack.end())
    if ( std::find(memoryTable.begin(), memoryTable.end(), ide) != memoryTable.end() )
    {
        cout << "ERROR: line" << yylineno << " Kolejna deklaracja zmiennej: " << ide << endl;
        exit(1);
    }
    else
    {
        Identifier s;
        createIdentifier(&s, ide, "IDE");
        memoryTable.push_back(ide);
        
    }
}

void expressRead()
{
    Identifier a = ideStack.top();
    ideStack.pop();
    pushCommand("GET B");
    memoryTable.push_back(a.name);
    storeInMemory("B", a.name);

}

void ideAsignExpress(string ide)
{
    storeInMemory("B", ide);

}
void expressWrite() {

    Identifier a = ideStack.top();
    ideStack.pop();
    loadFromMemory(a.name,"B");
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
        cout<<" ERROR"<<endl;
        return;
    }
    setRegister("A", placeInMemory);
    pushCommand("LOAD "+reg);
}

int findInVector(string var){

    for(int i = 0; i < memoryTable.size(); i++){
        //cout<<memoryTable[i].name<<endl;
        if(memoryTable[i] == var){
            return i;
        }
    }
    return -1;
}


