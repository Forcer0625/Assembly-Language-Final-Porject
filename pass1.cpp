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
#define SOURCE_FORMAT "%s\t%s\t%s\n" //failed
#define OPCODE_FORMAT "%s %c%c\n"
#define SPsize 7
#define MAX_SYMBOL_SIZE 512

typedef struct Symbol Symbol;
typedef struct Operation Operation;
typedef struct SourceLine SourceLine;

char HeaderName[16];
char START_Address[16];

class SymbolTable
{
    private : 
        struct Symbol
        {
            char label[16];
            char address[16];
        };
        Symbol *member;
    public : 
        int size;
        void build(int _size)
        {
            size = _size;
            member = (Symbol*)malloc(sizeof(Symbol) * size);
        }
        void put(const char label[], const char address[])
        {

        }
};

class SourceTransformer
{
    private :
        struct SourceLine
        {
            unsigned int loc;
            char symbol[16];
            char instruction[16];
            char address[16];
            bool n,x,b,p,e;     //to implement SIX/XE
        };
        SymbolTable* table = NULL;
        SourceLine*  statement = NULL;
    public : 
        SourceTransformer() = default;
        int size; //not include start and end
        int symbolnum; //number of symbols in this program
        void Source_Read(FILE *input)   //Create source statement
        {
            if(!input)  //for source file open failed
                exit(1);

            //store header and start address
            char temp[8];
            char firstsmbol[8];
            char c;
            fscanf(input,SOURCE_FORMAT,HeaderName,temp,START_Address);

            //count size to create location and symbol
            do{if(c=fgetc(input) == '\n') size++;}while(c != EOF);
            fclose(input); input=fopen(SOURCE,"r");
            fscanf(input,SOURCE_FORMAT,HeaderName,temp,START_Address); //ignore start and header

            //Create loc counter & symbol
            statement = (SourceLine*)malloc(sizeof(SourceLine) * size);
            size=0; symbolnum=0;
            char tempstr[3][16]={"","",""}; //to process via
            int i=0,j=0;
            do
            {
                c = fgetc(input);
                if(c == '\n')
                {
                    if(tempstr[1][0] == '+')
                    {
                        //format 4 => e=1
                    }
                    if(tempstr[0][0] == '\0')
                    {
                        //type 1 : [No Symbol]  instruction     address
                        
                    }
                    else
                    {
                        //type 2 : Symbol       instruction     address
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
                //system("cls");
            }while(strcmp(tempstr[1],"END") != 0);

            fclose(input); input=fopen(SOURCE,"r");
            fscanf(input,SOURCE_FORMAT,HeaderName,temp,START_Address); //ignore start and header

            

            //Create a symbol table by size
            if(symbolnum > 0)
            {
                table = (SymbolTable*)malloc(sizeof(SymbolTable));
                table -> build(symbolnum);
            }// symbolnum = 0, which means there is no symbol in this program


            

        }

        unsigned int IntQuery(const char symbolname[])
        {
            

            return -1;//not exist in symbol table
        }
};

class OperationCode
{
    private : 
        unsigned int StrAdd_To_unsiginedInt(const char address[])   //only work for 2 hex bit 
        {
            unsigned int re=0;
            switch (address[0])
            {
                case 'A':
                    re+=(10*16);
                    break;
                case 'B':
                    re+=(11*16);
                    break;
                case 'C':
                    re+=(12*16);
                    break;
                case 'D':
                    re+=(13*16);
                    break;
                case 'E':
                    re+=(14*16);
                    break;
                case 'F':
                    re+=(15*16);
                    break;
                default:
                    re+=((address[0] - '0') * 16);
                    break;
            }
            switch (address[1])
            {
                case 'A':
                    re+=10;
                    break;
                case 'B':
                    re+=11;
                    break;
                case 'C':
                    re+=12;
                    break;
                case 'D':
                    re+=13;
                    break;
                case 'E':
                    re+=14;
                    break;
                case 'F':
                    re+=15;
                    break;
                default:
                    re+=address[1] - '0';
                    break;
            }
            return re;
        }
        struct Operation
        {
            char opname[16];
            unsigned int code;   // ! : refill 0 for code < 16 
            char codestr[3];// represent opcode as a string
        };
        Operation *operation;
        const char special_case[SPsize][8]={"START","END","WORD","RESW","BYTE","RESB","C\'EOF\'"};
    public :
        OperationCode() = default;
        int size;
        void OPcode_Read(FILE *input)   //Create operation table
        {
            if(!input)  //for operation code file open failed
                exit(1);
            
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
        int query(const char name[])    //to query a instruction and retrun its code, if not exist, return -1
        {
            for(int i=0;i<size;i++) //linear scan
                if(strcmp(name, operation[i].opname) == 0)
                    return operation[i].code;

            //special case : START END WORD RESW BYTE RESB
            for(int i=0;i<SPsize;i++)
                if(strcmp(name,special_case[i]))
                    return -2;

            return -1;
        }
};

int main()
{
    OperationCode opcode;
    opcode.OPcode_Read(fopen(OPCODE,"r"));
    int t=opcode.query("J");
    printf("\n%X\n",t);

    /*char str1[10],str2[10],str3[10];
    FILE* SOURCEinput=fopen(SOURCE,"r");
    for(int i=0;i<4;i++)
    {
        fscanf(SOURCEinput,SOURCE_FORMAT,str1,str2,str3);
        printf(SOURCE_FORMAT,str1,str2,str3);
    }*/
    SourceTransformer st;
    st.Source_Read(fopen(SOURCE,"r"));

    //printf("\ntransfered : %d\n",StrAdd_To_unsiginedInt("3C"));

    system("PAUSE");
    return 0;
}