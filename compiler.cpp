#include "compiler.hpp"

map<string, Identifier> identifierStack;
map<string, string> registerMap;
stack<Identifier> ideStack;
vector<string> codeStack;
stack<string> regStack;
vector <Identifier> memoryTable;

int programCounter = 0;
string currentRegister = "";
int assignFlag = 1;
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

Identifier getValueIndentifier(string name)
{

    Identifier ide = identifierStack.at(name);
    return ide;
}

string getRegister()
{

    if (identifierStack.size() < 5)
    {
        string x = regStack.top();
        regStack.pop();
        return x;
    }
    return "";
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
    if ((b.type == "IDE" && a.type == "NUM"))
    {

        for (int i = 0; i < std::stoi(a.name); i++)
        {
            pushCommand("DEC " + registerMap.at(b.name));
        }
    }
    if (b.type == "NUM" && a.type == "IDE")
    {
        //co tu zrobić
    }
    if (a.type == "IDE" && b.type == "IDE")
    {
        pushCommand("SUB " + registerMap.at(a.name) + " " + registerMap.at(b.name));
    }
}

void multpO(Identifier a, Identifier b, Identifier c)
{

    registerMap["temp1"] = getRegister();
    
    if ((b.type == "IDE" && a.type == "IDE"))
    {
        // pushCommand("SUB " + registerMap.at("temp1") + " " + registerMap.at("temp1"));
        

    }

    if ((b.type == "IDE" && a.type == "NUM" && c.type == "IDE"))
    {
        pushCommand("COPY " + registerMap.at("temp") + " " + registerMap.at(b.name));
        pushCommand("COPY " + registerMap.at(c.name) + " " + registerMap.at(b.name));

        for (int i = 1; i < std::stoi(a.name); i++)
        {
            pushCommand("ADD " + registerMap.at(c.name) + " " + registerMap.at("temp"));
        }
    }
}

void divideO(Identifier a, Identifier b, Identifier c)
{

    if ((b.type == "IDE" && a.type == "IDE"))
    {
        
        pushCommand("SUB " + registerMap.at("temp") + " " + registerMap.at("temp"));
        pushCommand("COPY " + registerMap.at("temp2") + " " + registerMap.at(b.name));
        pushCommand("SUB " + registerMap.at("temp2") + " " + registerMap.at(b.name));
        string jzeroPosition = to_string(programCounter);
        string jumpPosition1 = to_string(programCounter + 6);
        string jumpPosition2 = to_string(programCounter + 9);
        pushCommand("JZERO " + registerMap.at("temp2") + " " + jumpPosition1);

        pushCommand("JZERO " + registerMap.at(b.name) + " " + jumpPosition2);
        string jumpPosition3 = to_string(programCounter);
        pushCommand("JZERO " + registerMap.at(a.name) + " " + jumpPosition2);
        pushCommand("SUB " + registerMap.at(a.name) + " " + registerMap.at(b.name));
        pushCommand("INC " + registerMap.at("temp"));
        pushCommand("JUMP " + jzeroPosition);
        pushCommand("COPY " + registerMap.at("temp2") + " " + registerMap.at(b.name));
        pushCommand("SUB " + registerMap.at("temp2") + " " + registerMap.at(a.name));
        pushCommand("JZERO " + registerMap.at("temp2") + " " + jumpPosition3);

      }

    if ((b.type == "IDE" && a.type == "NUM" && c.type == "IDE"))
    {
        pushCommand("COPY " + registerMap.at("temp") + " " + registerMap.at(b.name));
        pushCommand("COPY " + registerMap.at(c.name) + " " + registerMap.at(b.name));

        for (int i = 1; i < std::stoi(a.name); i++)
        {
            pushCommand("ADD " + registerMap.at(c.name) + " " + registerMap.at("temp"));
        }
    }
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
    if (identifierStack.find(ide) != identifierStack.end())
    {
        cout << "ERROR: line" << yylineno << " Kolejna deklaracja zmiennej: " << ide << endl;
        exit(1);
    }
    else
    {
        Identifier s;
        createIdentifier(&s, ide, "IDE");
        if (identifierStack.count(ide) == 0)
        {
            identifierStack.insert(make_pair(ide, s));
            numberofVariables++;
        }
    }
}

void expressRead()
{
    Identifier a = ideStack.top();
    ideStack.pop();
    pushCommand("GET B");
    memoryTable.push_back(a);
    storeInMemory("B", a.name);

}

void ideAsignExpress(string ide)
{
    if (assignFlag == 1)
    {
        Identifier a = identifierStack.at(ide);
        storeInMemory("B",a.name);
    }
    if (assignFlag == 0)
    {

        currentRegister = registerMap.at(ide);
    }
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
    subO(a, b);

}
void multp(string ide1, string ide2, int yylineo){

    assignFlag = 0;
    Identifier a = ideStack.top();
    ideStack.pop();
    Identifier b = ideStack.top();
    ideStack.pop();
    Identifier c = ideStack.top();
    ideStack.pop();
    multpO(a,b,c);

}
void divide(string ide1, string ide2, int yylineo){
    cout<<"DIVISION"<<endl;
    assignFlag = 0;
        Identifier a = ideStack.top();
        ideStack.pop();
        Identifier b = ideStack.top();
        ideStack.pop();
        Identifier c = ideStack.top();
        ideStack.pop();
        divideO(a,b,c);
}

void getNumber(string num){
    Identifier s;
    createIdentifier(&s, num, "NUM");
    ideStack.push(s);
}

void getIdentifier(string ide){

    Identifier s = identifierStack.at(ide);
    // if(registerMap.find(s.name) == registerMap.end())
    //     registerMap[s.name] = getRegister(); 
    ideStack.push(s);
    // cout<<"getIde "<<s.name<<endl;
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
        if(memoryTable[i].name == var){
            return i;
        }
    }
    return -1;
}


