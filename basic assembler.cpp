#include<cstdlib>
#include<cstdio>
#include<iostream>
#include<string.h>
using namespace std;

#define SOURCE "source.txt"
#define OPCODE "opcode.txt"
#define OUTPUT1 "PASS1_location_counter.txt"
#define OUTPUT2 "PASS1_symbol_table.txt"
#define OUTPUT3 "PASS2_object_code.txt"
#define SOURCE_FORMAT "%s\t%s\t%X\n" //failed
#define OPCODE_FORMAT "%s %c%c\n"
#define SYMBOL_FORMAT "%s\t%X\n"
#define C_EOF "454F46"

unsigned int pow(unsigned int base,int power)
{
    if (power != 0)
        return (base * pow(base, power - 1));
    else
        return 1;
}
unsigned int StrToDec(const char* input)
{
    unsigned int re=0;
    int len = strlen(input);
    int index = 0;
    while(len != 0)
    {
        char c = input[index++];
        unsigned int temp = c - '0';
        re+=temp*pow(10,--len);
    }
    return re;
}
unsigned int StrToInt(const char* input)
{
    unsigned int re=0;
    int len = strlen(input);
    int index = 0;
    while(len != 0)
    {
        char c = input[index++];
        unsigned int temp;
        switch(c)
        {
            case 'A':temp=  10 ;break;
            case 'B':temp=  11 ;break;
            case 'C':temp=  12 ;break;
            case 'D':temp=  13 ;break;
            case 'E':temp=  14 ;break;
            case 'F':temp=  15 ;break;
            default :temp=c-'0';break;
        }
        re+=(temp << (--len)*4);
    }
    return re;
}
struct Operation
{
    char opname[16];
    unsigned int code;   // ! : refill 0 for code < 16 
    char codestr[3];// represent opcode as a string
};
class OperationCode
{
    private : 
        Operation *operation;
    public :
        OperationCode() = default;
        int size;
        void build(const char* _OpcodeFileName)   //Create operation table
        {
            FILE* input=fopen(_OpcodeFileName,"r");
            //for operation code file open failed
            if(!input) {printf("file %s not found.\n",_OpcodeFileName);system("PAUSE");exit(1);}
            
            size=0;     //how many operations to store
            char str[32];       //temp for count;
            char num1,num2;          //temp for count;
            while(fscanf(input,OPCODE_FORMAT,str,&num1,&num2)!=EOF) size++;
            fclose(input);

            operation=(Operation*)malloc(sizeof(Operation) * size); //memory operations
            
            //store Instruction names and their operation code
            input=fopen(OPCODE,"r"); //open again for real stroe
            for(int i=0;i<size;i++)
            {
                fscanf(input,OPCODE_FORMAT,str,&num1,&num2);
                strcpy(operation[i].opname,str);            //store operation name
                operation[i].codestr[0]=num1;               //store operation code as a string
                operation[i].codestr[1]=num2;
                operation[i].codestr[2]='\0';
                operation[i].code=                          //store operation code
                StrAdd_To_unsiginedInt(operation[i].codestr); 

            }
            
        }
        Operation* query(const char name[])    //to query a instruction and retrun its code, if not exist, return -1
        {
            for(int i=0;i<size;i++) //linear scan
                if(strcmp(name, operation[i].opname) == 0)
                    return &(operation[i]);

            return NULL;
        }
        void del()
        {
            if(operation)
                free(operation);
            free(this);
        }
};
struct Symbol
{
    char* symbolname;
    unsigned int address;
    Symbol* next;
};
class SymbolTable
{
    private :
        Symbol* head;
        Symbol* Symbol_new(const char* name,unsigned int address)
        {
            Symbol* SNode=(Symbol*)malloc(sizeof(Symbol));
            if(!SNode)
                return SNode;

            SNode->symbolname=(char*)malloc(sizeof(char)*strlen(name));
            //SNode->address=(char*)malloc(sizeof(char)*strlen(address));
            strcpy(SNode->symbolname,name);
            SNode->address=address;
            //strcpy(SNode->address,address);
            SNode->next=NULL;

            return SNode;
        }
        
    public :
        void build()
        {
            head=NULL;
        }
        Symbol* query(const char* target)
        {
            for(Symbol* temp=head;temp;temp=temp->next)
                if(!strcmp(temp->symbolname,target))
                    return temp;
            return NULL;
        }
        void addSymbol(const char* name,unsigned int address)
        {
            Symbol* SNode=Symbol_new(name,address);
            SNode->next=head;
            head=SNode;
        }
        bool isEmpty()
        {
            return head==NULL;
        }
        void write(const char* _SymbolTableFileName)
        {

        }
        Symbol* findLast()
        {   
            Symbol* temp;
            for(temp=head;temp->next;temp=temp->next);
            return temp;
        }
        void del()
        {
            while(head)
            {
                Symbol* temp=head->next;
                free(head);
                head=temp;
            }
            free(this);
        }

};
//This Function will create program location and symbol table.
void Pass1(const char* _SourceFileName)
{
    //Create Operation Table and Symbol Table
    OperationCode* OpTable=(OperationCode*)malloc(sizeof(OperationCode));
    SymbolTable* SymTable=(SymbolTable*)malloc(sizeof(SymbolTable));
    OpTable ->build(OPCODE);
    SymTable->build();

    FILE* input  = fopen(_SourceFileName,"r");
    if(!input) {printf("file %s not found.\n",_SourceFileName);system("PAUSE");exit(1);}
    FILE* output1= fopen(OUTPUT1,"w");//LocationCount
    FILE* output2= fopen(OUTPUT2,"w");//Symbol Table

    char tempstr[3][16]={"","",""}; //to process via
    /*
     *  tempstr[0] for Symbol
     *  tempstr[1] for Opcode(Instruction)
     *  tempstr[2] for Operand
     */
    char c; int i=0,j=0; 
    unsigned int LOCCTR;    //LocationCounter
    unsigned int START_Address;
    do
    {
        c = fgetc(input);
        if(c == '\n')
        {
            if(strcmp(tempstr[1],"START") == 0) //Opcode=START
            {
                START_Address = LOCCTR = StrToInt(tempstr[2]);
                //initialize LOCCTR to START address
                fprintf(output1,"%X\t%s\t%s\t%s\n"
                ,LOCCTR,tempstr[0],tempstr[1],tempstr[2]);
                //Write line to initermediate file : OUTPUT1
            }
            else
            {
                //Write Line to record LocationCount
                fprintf(output1,"%X\t%s\t%s\t%s\n",LOCCTR,tempstr[0],tempstr[1],tempstr[2]);
                //Label process START
                if(tempstr[0][0] != '\0')   //if there is a symbol in the LABEL ffield
                {
                    Symbol* found=SymTable->query(tempstr[0]);
                    if(found)
                    {
                        //Error:Duplicate Symbol
                        printf("Duplicate Symbol:%s\n",tempstr[0]);
                        system("PAUSE");
                        exit(1);
                    }
                    else
                    {
                        SymTable->addSymbol(tempstr[0],LOCCTR);
                        fprintf(output2,"%s\t%X\n",tempstr[0],LOCCTR);
                    }
                }
                //Label process END

                //LocationCount process START
                Operation* found=OpTable->query(tempstr[1]);
                if(found)
                {
                    unsigned int instruction_length=strlen(found->codestr)>>1;//two hex bit for one byte
                    LOCCTR += 3*instruction_length;
                }
                else if(!strcmp(tempstr[1],"WORD"))
                    LOCCTR += 3;
                else if(!strcmp(tempstr[1],"RESW"))
                    LOCCTR += 3*StrToDec(tempstr[2]);
                else if(!strcmp(tempstr[1],"RESB"))
                    LOCCTR += StrToDec(tempstr[2]);
                else if(!strcmp(tempstr[1],"BYTE"))
                {
                    if(!strcmp("C\'EOF\'",tempstr[2]))
                        strcpy(tempstr[2],C_EOF);
                    else
                    {
                        char constant[5] = {'\0'};
                        int i=0,len=strlen(tempstr[2]);
                        for(;i<len;i++) if(tempstr[2][i]=='\'') break;
                        for(int j=0;tempstr[2][++i]!='\'';j++) constant[j]=tempstr[2][i];
                        strcpy(tempstr[2],constant);
                    }
                    unsigned int constant_length=strlen(tempstr[2])>>1;//2 hex bit for 1 byte
                    LOCCTR += constant_length;
                }
                else
                {
                    //Error(invalid operation code)
                    printf("invalid operation code:%s\n",tempstr[1]);
                    system("PAUSE");
                    exit(1);
                }
                //LocationCount process END
            }
            

            //reset temp string
            for(j= 3;j>0;j--)
            for(i=16;i>0;i--)
            tempstr[j-1][i-1]='\0';
        }
        else
        {
            if(c == '\t')   {j++;continue;}
            sprintf(tempstr[j],"%s%c",tempstr[j],c);
        }
    }while(strcmp(tempstr[1],"END") != 0);
    fprintf(output1,"\t\t%s\t%s","END",SymTable->findLast()->symbolname);

    //delete Operation Table and Symbol Table
    OpTable ->del();
    SymTable->del();
}

int main()
{
    //OperationCode* OpTable=(OperationCode*)malloc(sizeof(OperationCode));
    //OpTable->OPcode_Read(fopen(OPCODE,"r"));
    //Operation* temp=OpTable->query("COMP");
    //printf("\n%X\n",temp->code);
    //printf("\ntransfered : %d\n",StrAdd_To_unsiginedInt("3C"));
    
    //unsigned int t=StrToDec("");
    //printf("%u\n",t);
    
    Pass1(SOURCE);

    system("PAUSE");
    return 0;
}