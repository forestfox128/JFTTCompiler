#include "compiler.hpp"

map<string, Identifier> identifierStack;
map<string, string> registerMap;
stack<Identifier> ideStack;
vector<string> codeStack;
stack<string> regStack;

int programCounter = 0;
string currentRegister = "";
long long int memCounter;
/*long long int registerValue;*/
long long int depth;
int assignFlag = 1;
int writeFlag;


void initializeStackReg()
{
    regStack.push("E");
    regStack.push("D");
    regStack.push("C");
    regStack.push("B");
}

void createIdentifier(Identifier *s, string name, long long int isLocal, long long int isArray, string type)
{
    s->name = name;
    s->type = type;
    s->initialized = 0;
    if (isLocal)
    {
        s->local = 1;
    }
    else
    {
        s->local = 0;
    }
    if (isArray)
    {
        s->tableSize = isArray;
    }
    else
    {
        s->tableSize = 0;
    }
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

        for (int i = 0; i < std::stoi(b.name); i++)
        {
            pushCommand("INC " + registerMap.at(a.name));
        }
    }
    if (a.type == "NUM" && b.type == "IDE")
    {
        for (int i = 0; i < std::stoi(a.name); i++)
        {
            pushCommand("INC " + registerMap.at(b.name));
        }
    }
    if (a.type == "IDE" && b.type == "IDE")
    {
        pushCommand("ADD " + registerMap.at(a.name) + " " + registerMap.at(b.name));
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

    registerMap["temp"] = getRegister();
    if ((b.type == "IDE" && a.type == "IDE"))
    {
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

    registerMap["temp"] = getRegister();
    if ((b.type == "IDE" && a.type == "IDE"))
    {

        pushCommand("COPY " + registerMap.at("temp") + " " + registerMap.at(a.name));
        string jzeroPosition = to_string(programCounter);
        string jumpPosition = to_string(programCounter + 4);
        pushCommand("JZERO " + registerMap.at(b.name) + " " + jumpPosition);
        pushCommand("ADD " + registerMap.at(a.name) + " " + registerMap.at("temp"));
        pushCommand("DEC " + registerMap.at(b.name));
        pushCommand("JUMP " + jzeroPosition);
        pushCommand("SUB " + registerMap.at(a.name) + " " + registerMap.at("temp"));
    }

    if ((b.type == "IDE" && a.type == "NUM" && c.type == "IDE"))
    {
        cout << "NUMENENE" << currentRegister << endl;
        pushCommand("COPY " + registerMap.at("temp") + " " + registerMap.at(b.name));
        pushCommand("COPY " + registerMap.at(c.name) + " " + registerMap.at(b.name));

        for (int i = 1; i < std::stoi(a.name); i++)
        {
            pushCommand("ADD " + registerMap.at(c.name) + " " + registerMap.at("temp"));
        }
    }
}

void addDeclaredVariable(string variable, Identifier i)
{
    if (identifierStack.count(variable) == 0)
    {
        identifierStack.insert(make_pair(variable, i));
        identifierStack.at(variable).counter = 0;
    }
    else
    {
        identifierStack.at(variable).counter = identifierStack.at(variable).counter + 1;
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
        createIdentifier(&s, ide, 0, 0, "IDE");
        addDeclaredVariable(ide, s);
    }
}

void expressRead()
{
    Identifier a = ideStack.top();
    ideStack.pop();
    pushCommand("GET " + registerMap.at(a.name));
}
void ideAsignExpress(string ide)
{
    if (assignFlag == 1)
    {
        Identifier a = ideStack.top();
        ideStack.pop();
        Identifier b = ideStack.top();
        ideStack.pop();
        pushCommand("COPY " + registerMap.at(b.name) + " " + registerMap.at(a.name));
    }
    if (assignFlag == 0)
    {

        currentRegister = registerMap.at(ide);
        cout << "Identif " << currentRegister << endl;
    }
}
void expressWrite() {

    Identifier a = ideStack.top();
    ideStack.pop();
    pushCommand("PUT " + currentRegister);
}

void add(string ide1, string ide2, int yylineo){

    Identifier a = ideStack.top();
    ideStack.pop();
    Identifier b = ideStack.top();
    ideStack.pop();
    cout << " " << a.name << " " << b.name << endl;
    addO(a, b);

    

}
void sub(string ide1, string ide2, int yylineo){

    Identifier a = ideStack.top();
    ideStack.pop();
    Identifier b = ideStack.top();
    ideStack.pop();
    cout << " " << a.name << " " << b.name << endl;
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
    createIdentifier(&s, num, 0, 0, "NUM");
    ideStack.push(s);
}

void getIdentifier(string ide){

    Identifier s = getValueIndentifier(ide);
    if(registerMap.find(s.name) == registerMap.end())
        registerMap[s.name] = getRegister(); 
    ideStack.push(s);
}

