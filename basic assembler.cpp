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
#define OUTPUT4 "PASS2_final_object_program.txt"
#define SOURCE_FORMAT "%s\t%s\t%X\n"
#define OPCODE_FORMAT "%s %c%c\n"
#define SYMBOL_FORMAT "%s\t%X\n"


unsigned int pow(unsigned int base,int power)
{
    if (power != 0)
        return (base * pow(base, power - 1));
    else
        return 1;
}
char* HexToStr(unsigned int input)
{
    char* re=(char*)malloc(sizeof(char)*5); //4+1('\0')
    //char re[5];
    re[0]=re[1]=re[2]=re[3]=re[4]='\0';     //initialize
    re[0]=  input/pow(16,3);
            input-=(re[0]*pow(16,3));
    re[1]=  input/pow(16,2);
            input-=(re[1]*pow(16,2));
    re[2]=  input/16;
            input-=(re[2]*16);
    re[3]=  input;

    int ind=3;
    while(ind>=0)
    {
        char c=re[ind];
        switch (c)
        {
            case 10:
                re[ind]='A';
                break;
            case 11:
                re[ind]='B';
                break;
            case 12:
                re[ind]='C';
                break;
            case 13:
                re[ind]='D';
                break;
            case 14:
                re[ind]='E';
                break;
            case 15:
                re[ind]='F';
                break;
            default:
                re[ind]+='0';
                break;
        }
        ind--;
    }

    return re;
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
unsigned int StrToHex(const char* input)
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
                StrToHex(operation[i].codestr); 

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
        void build(const char* _SymbolTableFileName)
        {
            head=NULL;

            FILE* input=fopen(_SymbolTableFileName,"r");
            if(!input) {printf("%s file not found.\n",_SymbolTableFileName);system("PAUSE");exit(1);}
            char tempstr[16]={'\0'}; //Symbol name
            unsigned int address;    //Symbol location

            while(fscanf(input,SYMBOL_FORMAT,tempstr,&address)!=EOF)
                addSymbol(tempstr,address);
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
class HeaderRecord
{
    /*
     * Header record:
     * Col.1        H
     * Col.2-7      Program name
     * Col.8-13     Starting address (hex)
     * Col.14-19    Length of object program in bytes (hex)
     */
    private:
        char head;
        char programName    [7]={' '};
        char startingAddress[7]={'0'};
        char lengthInByte   [7]={'0'};
    public :
    void build(const char* _programName,const char* _startAddress,const char* _length)
    {
        head='H';
        for(int i=0;i<6;i++)
        {
            programName     [i]=' ';
            startingAddress [i]='0';
            lengthInByte    [i]='0';
        }
        programName     [6]='\0';
        startingAddress [6]='\0';
        lengthInByte    [6]='\0';
        recordName  (_programName);
        recordStart (_startAddress);
        recordLength(_length);
    }
    void recordName(const char* _programName)
    {
        int len=strlen(_programName);
        for(int i=0;i<len;i++)
            programName[i]=_programName[i];
    }
    void recordStart(const char* _startAddress)
    {
        int len=strlen(_startAddress);
        int ind=5;
        for(int i=len-1;i>=0;i--)
            startingAddress[ind--]=_startAddress[i];
    }
    void recordLength(const char* _length)
    {
        int len=strlen(_length);
        int ind=5;
        for(int i=len-1;i>=0;i--)
            lengthInByte[ind--]=_length[i];
    }
    void write(FILE* output)
    {
        fprintf(output,"%c%s%s%s\n",head,programName,startingAddress,lengthInByte);
    }
    void del()
    {
        free(this);
    }
};
class TextRecord
{
    /*
     * Text record:
     * Col.1       T
     * Col.2-7     Starting address in this record (hex)
     * Col.8-9     Length of object code in this record in bytes
     * Col.10-69   Object code (hex) (2 columns per byte)
     */
    private:
        char head;
        char startingAddress[7];
        char length[3];
        char objectCode[61];
    public :
    int  ind;           //to record the capacity of objecCode[]
    void initialize()
    {
        head='T';
        for(int i=0;i<6;i++)
            startingAddress [i]='0';
            startingAddress [6]='\0';
        for(int i=0;i<61;i++)
            objectCode[i]='\0';
        length[0]=length[1]='0';
        length[2]='\0';
        ind=0;
    }
    void recordStart(const char* _startAddress)
    {
        int len=strlen(_startAddress);
        int ind=5;
        for(int i=len-1;i>=0;i--)
            startingAddress[ind--]=_startAddress[i];
    }
    char* getAddress()
    {
        return startingAddress;
    }
    void recordLength()
    {
        unsigned int _Length=strlen(objectCode)>>1;
        char* temp=HexToStr(_Length);
        length[0]=temp[2];
        length[1]=temp[3];
        free(temp);
    }
    void write(FILE* output)
    {
        fprintf(output,"%c%s%s%s\n",head,startingAddress,length,objectCode);
    }
    bool isFull(unsigned int length)
    {
        if(ind + length> 60)
            return true;
        return false;
    }
    void addObjcode(const char* obj)
    {
        ind+=strlen(obj);
        strcat(objectCode,obj);
    }
    void del()
    {
        free(this);
    }
};
class EndRecord
{
    /*
     * End record:
     * Col.1    E
     * Col.2~7  Address of first executable instruction (hex)
     */
    private:
        char head;
        char EndAddress[7];//Address of first executable instruction
    public :
    void build(const char* _EndAddress)
    {
        head='E';
        for(int i=0;i<6;i++)
            EndAddress[i]='0';
        EndAddress[6]='\0';
        recordAddress(_EndAddress);
    }
    void recordAddress(const char* _EndAddress)
    {
        int len=strlen(_EndAddress);
        int ind=5;
        for(int i=len-1;i>=0;i--)
            EndAddress[ind--]=_EndAddress[i];
    }
    void write(FILE* output)
    {
        fprintf(output,"%c%s",head,EndAddress);
    }
    void del()
    {
        free(this);
    }
};
//This Function will create program location and symbol table, then return program length.
unsigned int Pass1(const char* _SourceFileName)
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
                START_Address = LOCCTR = StrToHex(tempstr[2]);
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
                        //save symbol and its location
                        SymTable->addSymbol(tempstr[0],LOCCTR);
                        fprintf(output2,"%s\t%X\n",tempstr[0],LOCCTR);
                    }
                }
                //Label process END

                //LocationCount process START
                Operation* found=OpTable->query(tempstr[1]);
                if(found)
                {
                    //instruction=>LOCCTR add 3*instruction length (btye)
                    unsigned int instruction_length=strlen(found->codestr)>>1;//two hex bit for one byte
                    LOCCTR += 3*instruction_length;
                }
                else if(!strcmp(tempstr[1],"WORD"))
                    //WORD=>LOCCTR add 3 byte
                    LOCCTR += 3;
                else if(!strcmp(tempstr[1],"RESW"))
                    //RESW=>LOCCTR add 3*#[OPERAND] (byte)
                    LOCCTR += 3*StrToDec(tempstr[2]);
                else if(!strcmp(tempstr[1],"RESB"))
                    //RESB=>LOCCTR add #[OPERAND] (byte)
                    LOCCTR += StrToDec(tempstr[2]);
                else if(!strcmp(tempstr[1],"BYTE"))
                {
                    //BYTE=>convert constant length in byte
                    char readformat=tempstr[2][0];
                    unsigned int constant_length;
                    //find the constant in ' '
                    char constant[5] = {'\0'};
                    int i=0,len=strlen(tempstr[2]);
                    for(;i<len;i++) if(tempstr[2][i]=='\'') break;
                    for(int j=0;tempstr[2][++i]!='\'';j++) constant[j]=tempstr[2][i];
                    strcpy(tempstr[2],constant);

                    
                    if(/*read in char*/
                        readformat == 'C')
                        constant_length=strlen(tempstr[2]);     //1 char for 1 byte
                    else if(/*read in hex format*/
                        readformat == 'X')
                        constant_length=strlen(tempstr[2])>>1;  //2 hex bit for 1 byte

                    //add constant length
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
    //write last to listing file
    fprintf(output1,"\t\t%s\t%s","END",SymTable->findLast()->symbolname);

    //close file
    fclose(input);
    fclose(output1);
    fclose(output2);

    //delete Operation Table and Symbol Table
    OpTable ->del();
    SymTable->del();

    //return program length
    return LOCCTR-START_Address;
}
//This Function will create each statement's objetcode and object file.
void Pass2(const char* _LocationFileName,const char* _SymbolTableFileName,unsigned int _ProgramLength)
{
    //Create Operation Table and Symbol Table
    OperationCode*  OpTable =(OperationCode*)malloc(sizeof(OperationCode));
    SymbolTable*    SymTable=(SymbolTable*)  malloc(sizeof(SymbolTable));
    OpTable ->build(OPCODE);
    SymTable->build(_SymbolTableFileName);

    
    FILE* input  =fopen(_LocationFileName,"r");
    if(!input) {printf("file %s not found.\n",_LocationFileName);system("PAUSE");exit(1);}
    FILE* output3=fopen(OUTPUT3,"w");
    FILE* output4=fopen(OUTPUT4,"w");

    char tempstr[4][16]={"","",""}; //to process via
    /*
     *  tempstr[0] for Location(processed via pass1)
     *  tempstr[1] for Symbol
     *  tempstr[2] for Opcode(instruction)
     *  tempstr[3] for Operand
     */
    char c; int i=0,j=0;
    //Create a Text record to process
    TextRecord* text=(TextRecord*)malloc(sizeof(TextRecord));
    text->initialize();
    do
    {
        c = fgetc(input);
        if(c == '\n')
        {
            if(!strcmp(tempstr[2],"START"))
            {
                //Write lisitng line
                fprintf(output3,"%s\t%s\t%s\t%s\n",
                    tempstr[0],tempstr[1],tempstr[2],tempstr[3]);
                //Write Header recoed to final object file
                HeaderRecord* rec=(HeaderRecord*)malloc(sizeof(HeaderRecord));
                char*   temp=HexToStr(_ProgramLength);
                rec->build(tempstr[1],tempstr[3],temp);
                rec->write(output4);
                rec->del();
                free(temp);
                //give Text record starting address
                text->recordStart(tempstr[3]);
            }
            else
            {
                fprintf(output3,"%s\t%s\t%s\t%s",
                    tempstr[0],tempstr[1],tempstr[2],tempstr[3]);
                char objcode[7]={'\0'};
                Operation* found=OpTable->query(tempstr[2]);
                if(found)
                {
                    strcpy(objcode,found->codestr);
                    //if there is a symbol in OPERAMD field
                    if(tempstr[3][0]!='\0')
                    {
                        Symbol* symbol;

                        //remove ,X to get real symbol
                        bool indexed_address=false; //check if indexed addressing mode
                        for(int i=0;i<strlen(tempstr[3]);i++)
                        if(tempstr[3][i]==',')  {tempstr[3][i]='\0';indexed_address=true;break;}

                        if(symbol=SymTable->query(tempstr[3]))
                        {
                            //store symbol value as operand address
                            char* trans=HexToStr(symbol->address);
                            strcpy(tempstr[3],trans);
                            free(trans);
                            strcat(objcode,tempstr[3]);
                            if(indexed_address) objcode[2]+=0b1000;
                            /*  for indexed-addressing mode :
                             *  8 bits      1 bit   15 bits
                             *  [opcode]    [x]     [address]
                             *  so objcode[2] plus 0b1000
                             */
                        }
                        else
                        {
                            //Error:undefined symbol
                            printf("Error:undefined symbol %s\t",tempstr[3]);
                            system("PAUSE");
                            exit(1);
                        }
                    }
                    else
                    {
                        //store 0 as operand
                        strcat(objcode,"0000");
                    }
                    
                }
                else if(!strcmp("BYTE",tempstr[2]))
                {
                    char readformat=tempstr[3][0];

                    //find the constant in ' '
                    char constant[5] = {'\0'};
                    int i=0,len=strlen(tempstr[3]);
                    for(;i<len;i++) if(tempstr[3][i]=='\'') break;
                    for(int j=0;tempstr[3][++i]!='\'';j++) constant[j]=tempstr[3][i];
                    //strcpy(tempstr[3],constant);

                    //covert constant to object code
                    if(readformat == 'X')
                    {
                        strcat(objcode,constant);
                    }
                    else if(readformat == 'C')
                    {
                        for(int i=0;i<strlen(constant);i++)
                        {
                            char charformat[3]={'\0'};
                            char* convert=HexToStr(constant[i]);
                            //2 hex for 1 byte for 1 char
                            charformat[0]=convert[2];
                            charformat[1]=convert[3];
                            strcat(objcode,charformat);
                        }
                    }
                    
                }
                else if(!strcmp("WORD",tempstr[2]))
                {
                    //opreation refill 0 to high hex bit
                    objcode[0]='0';
                    objcode[1]='0';
                    //translate immediate value to opreand
                    char *trans=HexToStr(StrToDec(tempstr[3]));
                    strcat(objcode,trans);
                    free(trans);
                }
                
                fprintf(output3,"\t%s\n",objcode);

                //if object code won't fit into the current Text record
                if(text->isFull(strlen(objcode))||(objcode[0]=='\0'&&text->ind>0))
                {
                    //record length
                    text->recordLength();
                    //write line to final object file
                    text->write(output4);
                    //Initailize Text record
                    text->initialize();
                    //record next Starting Address
                    text->recordStart(tempstr[0]);
                }
                //add object code to Text record
                text->addObjcode(objcode);

            }
            

            //reset temp string
            for(j= 4;j>0;j--)
            for(i=16;i>0;i--)
            tempstr[j-1][i-1]='\0';
        }
        else
        {
            if(c == '\t')   {j++;continue;}
            sprintf(tempstr[j],"%s%c",tempstr[j],c);
        }
    }while(strcmp(tempstr[2],"END") != 0);
    //write last to lisiting file
    fprintf(output3,"\t\t%s\t%s","END",SymTable->findLast()->symbolname);

    //write last Text record to final object file
    text->recordLength();
    text->write(output4);
    text->del();

    //write End record to final object file
    char* endadd=HexToStr(SymTable->findLast()->address);
    EndRecord* end=(EndRecord*)malloc(sizeof(EndRecord));
    end->build(endadd);
    end->write(output4);
    free(endadd);

    //close files
    fclose(input);
    fclose(output3);
    fclose(output4);

    //delete Operation Table and Symbol Table
    OpTable ->del();
    SymTable->del();

}

int main()
{
    unsigned int _ProgramLength=Pass1(SOURCE);
    Pass2(OUTPUT1,OUTPUT2,_ProgramLength);

    system("PAUSE");
    return 0;
}