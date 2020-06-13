#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define headerMax 10
#define opcodeNameMax 7
#define opcodeFormatyMax 4
#define opcodeInfoMax 6
#define symtabNameMax 9
#define regtabNameMax 3
#define srcMax 33
#define srcTagMax 6
#define srcCodeMax 6
#define srcOperandMax 8
#define srcExtendTagIndex 7
#define srcOperTagIndex 15
#define srcOperatorIndex 24
#define regtabHeaderMax 9
#define blockTabMax 3

int use = 0;
struct regtab
{
    char name[regtabNameMax];
    int code;
};
struct regtab reg[9] = { {"A",0}, {"X",1}, {"L",2}, {"PC",8}, {"SW",9}, {"B",3}, {"S",4}, {"T",5}, {"F",6}, };
struct optab_Node
{
    char name[opcodeNameMax];
    char format[opcodeFormatyMax];
    char info[opcodeInfoMax];
    int opcode;
    struct optab_Node * next;
};
typedef struct optab_Node optab_node;
optab_node * optabHeader[headerMax];

struct symlit_Node
{
    char name[symtabNameMax];
    int loc;
    int addressFlag;
    int use;
    struct symlit_Node * next;
};
typedef struct symlit_Node symlit_node;
symlit_node * symtabHeader[headerMax];
symlit_node * littabHeader[headerMax];

struct block_Node
{
    char name[symtabNameMax];
    int start;
    int length;
};
typedef struct block_Node block_node;
block_node blockTab[blockTabMax];

struct t_Node
{
    int start;
    int length;
    int use;
    struct t_Node * next;
};
typedef struct t_Node t_node;

struct m_Node
{
    int loc;
    struct m_Node *next;
};
typedef struct m_Node m_node;
t_node *thead = NULL, *tptr;
m_node *mhead = NULL, *mptr;

void regtabPrint(void)
{
    printf("%14s\nRow REG_Name REG_Code\n", "REGTAB");
    for(int row = 1; row <= 9; row++)
    {
        printf("%2d%7s%8d\n", row, reg[row-1].name, reg[row-1].code);
    }
    printf("\n");
}

optab_node * optabNewNode(void)
{
    optab_node * add = malloc(sizeof(optab_node));
    add->next = NULL;
    return add;
}

void optabPrint(optab_node * head, int hash, int * row)
{
    optab_node * ptr;
    if(head->next == NULL)
    {
        printf("empty\n");
    }
    else
    {
        ptr = head->next;
        while(ptr != NULL)
        {
            (*row)++;
            printf("%2d%5d%7s%7s    %02X     %s\n", *row, hash, ptr->name, ptr->format, ptr->opcode, ptr->info);
            ptr = ptr->next;
        }
    }
}

void optabCreate(void)
{
    optab_node* ptr;
    int i, row = 0;
    for(i = 0; i < headerMax; i ++)
    {
        optabHeader[i] = optabNewNode();
    }
    FILE * fp_Optab = fopen("optab.txt", "r");
    int opcode;
    char name[opcodeNameMax], format[opcodeFormatyMax], info[opcodeInfoMax];
    while(1)
    {
        if(feof(fp_Optab) != 0) break;
        fscanf(fp_Optab, "%s %s %x %s", name, format, &opcode, info);

        ptr = optabHeader[key(name)];
        while(ptr->next != NULL)
        {
            ptr = ptr->next;
        }
        ptr->next = optabNewNode();
        ptr = ptr->next;
        strcpy(ptr->name, name);
        strcpy(ptr->format, format);
        ptr->opcode = opcode;
        strcpy(ptr->info, info);
    }
    fclose(fp_Optab);
    printf("%15s\nRow Hash Op_Name Format OpCode info\n", "OPTAB");
    for(i = 0; i < headerMax; i++)
    {
        optabPrint(optabHeader[i], i, &row);
    }
    printf("\n");
}

symlit_node * symlitNewNode(void)
{
    symlit_node * add = malloc(sizeof(symlit_node));
    add->addressFlag = 0;
    add->use = use;
    add->next = NULL;
    return add;
}

void symtabCreate(void)
{
    int i;
    for(i = 0; i < headerMax; i++)
    {
        symtabHeader[i] = symlitNewNode();
    }
}

void littabCreate(void)
{
    int i;
    for(i = 0; i < headerMax; i++)
    {
        littabHeader[i] = symlitNewNode();
    }
}

char *token(char temp[20])
{
    char * tok;
    if(temp[0] == ' ')
    {
        return NULL;
    }
    else
    {
        tok = strtok(temp, " ");
        return tok;
    }
}

t_node * tNewNode(int start, int use)
{
    t_node * add = malloc(sizeof(t_node));
    add->start = start;
    add->length = 0;
    add->use = use;
    add->next = NULL;
    return add;
}

void tbuild(int loc, int length, int flag, int use)
{
    if(thead == NULL)
    {
        thead = tNewNode(length - loc, use);
        tptr = thead;
    }
    else if(30 < tptr->length + loc || tptr->start + tptr->length + loc != length || flag)
    {
        tptr->next = tNewNode(length - loc, use);
        tptr = tptr->next;
        tptr->length += loc;
    }
    else
    {
        tptr->length += loc;
    }
}

int key(char name[opcodeNameMax])
{
    int i, sum = 0;
    for(i = 0; i < strlen(name); i++)
    {
        sum += name[i];
    }
    sum %= 10;
    return sum;

}

symlit_node * symlitFind(symlit_node * head, char tag[symtabNameMax])
{
    symlit_node * ptr = head;
    while(ptr != NULL)
    {
        if(!strcmp(ptr->name, tag))
        {
            return ptr;
        }
        ptr = ptr->next;
    }
    return NULL;
}

void symlitInsert(symlit_node * head, char name[symtabNameMax], int loc)
{
    symlit_node * ptr;
    ptr = head;
    while(ptr->next != NULL)
    {
        ptr = ptr->next;
    }
    ptr->next = symlitNewNode();
    ptr = ptr->next;
    strcpy(ptr->name, name);
    ptr->loc = loc;
}

m_node * mNewNode(int loc)
{
    m_node * add = malloc(sizeof(m_node));
    add->loc = loc;
    add->next = NULL;
    return add;
}

void mbuild(int loc)
{
    if(mhead == NULL)
    {
        mhead = mNewNode(loc+1);
        mptr = mhead;
    }
    else
    {
        mptr->next = mNewNode(loc+1);
        mptr = mptr->next;
    }
}

void littabAddressing(FILE* fp, symlit_node * head)
{
    symlit_node * ptr;
    if(head->next == NULL)
    {
        return;
    }
    else
    {
        ptr = head->next;
    }
    while(ptr != NULL)
    {
        if(!ptr->addressFlag)
        {
            ptr->loc = blockTab[use].length;
            ptr->addressFlag++;
            fprintf(fp, "%04X %d *      =%-25s\n", ptr->loc, use, ptr->name);
            if(ptr->name[0] == 'X')
            {
                blockTab[use].length += (strlen(ptr->name) - 3) / 2;
                ptr->use = use;
                tbuild((strlen(ptr->name) - 3) / 2, blockTab[use].length, 0, use);
            }
            else if(ptr->name[0] == 'C')
            {
                blockTab[use].length += strlen(ptr->name) - 3;
                ptr->use = use;
                tbuild(strlen(ptr->name) - 3, blockTab[use].length, 0, use);
            }
        }
        ptr= ptr->next;
    }
}

optab_node * optabFind(optab_node * head, char srcCode[srcCodeMax+1])
{
    optab_node * ptr = head;
    while(ptr != NULL)
    {
        if(!strcmp(ptr->name, srcCode))
        {
            return ptr;
        }
        ptr = ptr->next;
    }
    return NULL;
}

void tModify(void)
{
    tptr = thead;
    while(tptr != NULL)
    {
        tptr->start += blockTab[tptr->use].start;
        tptr = tptr->next;
    }
}

void mPrint(FILE* fp)
{
    mptr = mhead;
    while(mptr != NULL)
    {
        fprintf(fp, "\nM%06X05", mptr->loc);
//        printf("M%06X05\n", mptr->loc);
        mptr = mptr->next;
    }
}

void symlitPrint(symlit_node * head, int hash, int * row)
{
    symlit_node * ptr;
    if(head->next != NULL)
    {
        ptr = head->next;
        while(ptr != NULL)
        {
            (*row)++;
            printf("%2d%5d%8s   %04X %4d\n", *row, hash, ptr->name, ptr->loc, ptr->use);
            ptr = ptr->next;
        }
    }
}

void tPrint(FILE* fp, int address,int use, int length)
{
    if(tptr->length == 0 || blockTab[use].start + address + length > tptr->start + tptr->length || tptr->use != use )
    {
        tptr = tptr->next;
        while(tptr->length == 0)
        {
            tptr = tptr->next;
        }
        fprintf(fp, "\nT%06X%02X", tptr->start, tptr->length);
    }

}

int main()
{
//    system("chcp 65001");
    regtabPrint();
    optabCreate();
    symtabCreate();
    littabCreate();
    strcpy(blockTab[0].name, "DEFAULT");
    strcpy(blockTab[1].name, "NULL");
    strcpy(blockTab[2].name, "NULL");
    FILE * fp_input = fopen("srcpro.txt", "r");
//    FILE * fp_input = fopen("srcpro2.11.txt", "r");
    FILE * fp_output = fopen("intermediate.txt", "w");
    char srcStr[srcMax+5], temp[10], temp1[10], temp2[10], temp3[10];
    char *srcTag, *srcCode, *srcOperand, *srcOperand2;
    int flag = 0, srcOper, keyTemp, i, value, row = 0;
    optab_node * ptr;
    while(fgets(srcStr, srcMax, fp_input) != NULL)
    {
        if(flag)
        {
            flag--;
            continue;
        }
        flag++;
//        printf("%s\n", srcStr);
        strncpy(temp, srcStr, srcTagMax);
        srcTag = token(temp);
        strncpy(temp1, srcStr + srcTagMax + 2, srcCodeMax);
        srcCode = token(temp1);
        strncpy(temp2, srcStr + srcTagMax + 2 + srcCodeMax + 2, srcOperandMax);
        srcOperand = token(temp2);
        strncpy(temp3, srcStr + srcTagMax + 2 + srcCodeMax + 2 + srcOperandMax + 1, srcOperandMax);
        srcOperand2 = token(temp3);

        srcOper = atoi(srcOperand);
        if(!strcmp(srcCode, "START"))
        {
            blockTab[use].length = srcOper;
            blockTab[use].start = blockTab[use].length;
//            printf("%04X %s\n", locctr, srcStr);
            fprintf(fp_output, "%04X %d %s\n", blockTab[use].length, use, srcStr);
            thead = tNewNode(blockTab[use].length, use);
            tptr = thead;
            continue;
        }
//        else
//        {
//            locctr = 0;
//        }
        if(!strcmp(srcCode, "USE"))
        {
            if(srcOperand == NULL)
            {
                use = 0;
            }
            else
            {
                for(i = 0; i < blockTabMax; i++)
                {
                    if(!strcmp(blockTab[i].name, "NULL"))
                    {
                        strcpy(blockTab[i].name, srcOperand);
                        blockTab[i].length = 0;
                        use = i;
                        break;
                    }
                    else if(!strcmp(blockTab[i].name, srcOperand))
                    {
                        use = i;
                        break;
                    }
                }
            }
            fprintf(fp_output, "%04X %d %s\n", blockTab[use].length, use, srcStr);
            tbuild(0, blockTab[use].length, 1, use);

        }
        else if(strcmp(srcCode, "END") != 0)
        {
            if(srcTag != NULL)
            {
                keyTemp = key(srcTag);
                if(symlitFind(symtabHeader[keyTemp], srcTag) == NULL)
                {
                    if(!strcmp(srcCode, "EQU"))
                    {
                        if(srcStr[srcOperatorIndex] == ' ')
                        {
                            if(!strcmp(srcOperand, "*"))
                            {
                                value = blockTab[use].length;
                            }
                        }
                        else if(srcStr[srcOperatorIndex] == '+')
                        {
                            value = symlitFind(symtabHeader[key(srcOperand)], srcOperand)->loc +
                                    symlitFind(symtabHeader[key(srcOperand2)], srcOperand2)->loc;
                        }
                        else if(srcStr[srcOperatorIndex] == '-')
                        {
                            value = symlitFind(symtabHeader[key(srcOperand)], srcOperand)->loc -
                                    symlitFind(symtabHeader[key(srcOperand2)], srcOperand2)->loc;
                        }
                        symlitInsert(symtabHeader[keyTemp], srcTag, value);
                    }
                    else
                    {
                        symlitInsert(symtabHeader[keyTemp], srcTag, blockTab[use].length);
                    }

                }
                else
                {
                    printf("輸入檔有重複符號\n");
                }
            }

            if(!strcmp(srcCode, "EQU"))
            {
                fprintf(fp_output, "%04X %d %s \n", value, use, srcStr);
            }
            else
            {
                fprintf(fp_output, "%04X %d %s\n", blockTab[use].length, use, srcStr);
            }
            if(!strcmp(srcCode, "WORD"))
            {
                blockTab[use].length += 3;
            }
            else if(!strcmp(srcCode, "RESW"))
            {
                blockTab[use].length += 3 * srcOper;
            }
            else if(!strcmp(srcCode, "RESB"))
            {
                blockTab[use].length += srcOper;
            }
            else if(!strcmp(srcCode, "BYTE"))
            {
                if(*srcOperand == 'X')
                {
                    blockTab[use].length += (strlen(srcOperand) - 3) / 2;
                    tbuild((strlen(srcOperand) - 3) / 2, blockTab[use].length, 0, use);
                }
                else if(*srcOperand == 'C')
                {
                    blockTab[use].length += strlen(srcOperand) - 3;
                    tbuild(strlen(srcOperand) - 3, blockTab[use].length, 0, use);
                }

            }
            else if(srcStr[srcExtendTagIndex] == '+')
            {
                if(srcStr[srcOperTagIndex] != '#')
                {
                    mbuild(blockTab[use].length);
                }
                blockTab[use].length += 4;
                tbuild(4, blockTab[use].length, 0, use);
            }
//            else if(!strcmp(srcCode, "EQU"))
//            {
//
//            }
            else if(!strcmp(srcCode, "LTORG"))
            {
                for(i = 0; i < headerMax; i++)
                {
                    littabAddressing(fp_output, littabHeader[i]);
                }
            }
            else
            {
                ptr = optabFind(optabHeader[key(srcCode)], srcCode);
                if(ptr != NULL)
                {
                    if(!strcmp(ptr->format, "1"))
                    {
                        blockTab[use].length += 1;
                        tbuild(1, blockTab[use].length, 0, use);
                    }
                    else if(!strcmp(ptr->format, "2"))
                    {
                        blockTab[use].length += 2;
                        tbuild(2, blockTab[use].length, 0, use);
                    }
                    else
                    {
                        blockTab[use].length += 3;
                        tbuild(3, blockTab[use].length, 0, use);
                    }
                }

            }
            if(srcStr[srcOperTagIndex] == '=')
            {
                keyTemp = key(srcOperand);
                if(symlitFind(littabHeader[keyTemp], srcOperand) == NULL)
                {
                    symlitInsert(littabHeader[key(srcOperand)], srcOperand, 0);
                }
            }


        }
        else   //end
        {
            fprintf(fp_output, "%04X %d %s\n", blockTab[use].length, use, srcStr);
            for(i = 0; i < headerMax; i++)
            {
                littabAddressing(fp_output, littabHeader[i]);
            }
        }
    }//while
    for(i = 0; i < blockTabMax-1; i++)
    {
        blockTab[i+1].start = blockTab[i].start + blockTab[i].length;
    }
    tModify();
    tptr = thead;
    printf("%15s\nRow Hash SymName Address Use\n", "SYMTAB");
    for(i = 0; i < headerMax; i++)
    {
        symlitPrint(symtabHeader[i], i, &row);
    }
    printf("\n\n%15s\nRow Hash LitName Address Use\n", "LITTAB");
    row = 0;
    for(i = 0; i < headerMax; i++)
    {
        symlitPrint(littabHeader[i], i, &row);
    }
    printf("\n\n%40s\nRow addr/use%8s%47s\n", "Original Program <literal pool>", "Code", "Target Address");
    row = 0;
    fclose(fp_input);
    fclose(fp_output);

//--------------------------------------------------
    fp_input = fopen("intermediate.txt", "r");
    fp_output = fopen("D0746323_OBJFILE.txt", "w");
    int address, ta = 0;
    symlit_node * base = NULL;
    int plus;
    while(1)
    {

        ta = 0;
        if(flag)
        {
            flag--;
            continue;
        }
        flag++;
        row++;
        fscanf(fp_input, "%X %d", &address, &use);
        if(feof(fp_input) != 0) break;  //warning
        fgets(srcStr, srcMax, fp_input);
//        printf("%d %04X %s\n", row, address, srcStr);
        strncpy(temp, srcStr + 1, srcTagMax);
        srcTag = token(temp);
        strncpy(temp1, srcStr + 1 + srcTagMax + 2, srcCodeMax);
        srcCode = token(temp1);
        strncpy(temp2, srcStr + 1 + srcTagMax + 2 + srcCodeMax + 2, srcOperandMax);
        srcOperand = token(temp2);
        strncpy(temp3, srcStr + 1 + srcTagMax + 2 + srcCodeMax + 2 + srcOperandMax + 1, srcOperandMax);
        srcOperand2 = token(temp3);
        if(!strcmp(srcCode, "START"))
        {
            fprintf(fp_output, "H%-6s%06X%06X\nT%06X%02X", srcTag, blockTab[0].start, blockTab[blockTabMax-1].start + blockTab[blockTabMax-1].length, thead->start, thead->length);     //改
            printf("%2d  %04X %d %s\n", row, address, use, srcStr);
            continue;
        }

        if(strcmp(srcCode, "END") != 0)
        {

            ptr = optabFind(optabHeader[key(srcCode)], srcCode);
            if(ptr != NULL)
            {
                //code
                if(!strcmp(ptr->format, "3/4"))
                {
                    if(srcStr[srcExtendTagIndex+1] == '+')  //4
                    {
                        if(srcStr[srcOperTagIndex + 1] == '#')
                        {
                            ta += (ptr->opcode + 1) * (int)pow(16,6) ;
                        }
                        else if(srcStr[srcOperTagIndex + 1] == '@')
                        {
                            ta += (ptr->opcode + 2) * (int)pow(16,6);
                        }
                        else
                        {
                            ta += (ptr->opcode + 3) * (int)pow(16,6);
                        }
                        ta += 1 * (int)pow(16,5);
                        ta += symlitFind(symtabHeader[key(srcOperand)], srcOperand)->loc;
                        printf("%2d  %04X %d %s          %08X\n", row, address, use, srcStr, ta);
                        tPrint(fp_output, address, use, 4);
                        fprintf(fp_output, "%08X", ta);
                        continue;
                    }
                    else    //3
                    {
                        if(srcStr[srcOperTagIndex + 1] == '#')
                        {
                            ta += (ptr->opcode + 1) * (int)pow(16,4);
                        }
                        else if(srcStr[srcOperTagIndex + 1] == '@')
                        {
                            ta += (ptr->opcode + 2) * (int)pow(16,4);
                        }
                        else
                        {
                            ta += (ptr->opcode + 3) * (int)pow(16,4);
                        }
                        if(srcOperand2 != NULL)         //nixbpe
                        {
                            if(!strcmp(srcOperand2, "X"))
                            {
                                ta += 8 * (int)pow(16,3);
                            }
                        }
                        if(srcStr[srcOperTagIndex + 1] == '#' && *srcOperand <= '9' && *srcOperand >= '0' )
                        {
                            ta += atoi(srcOperand);
                        }
                        else if(srcStr[srcOperTagIndex+1] == '=')
                        {
                            if(base == NULL)
                            {
                                ta += 2 * (int)pow(16,3);
                                ta += ((symlitFind(littabHeader[key(srcOperand)], srcOperand)->loc + blockTab[symlitFind(littabHeader[key(srcOperand)], srcOperand)->use].start - address - 3) % (int)pow(16,3) );
                                if(symlitFind(littabHeader[key(srcOperand)], srcOperand)->loc + blockTab[symlitFind(littabHeader[key(srcOperand)], srcOperand)->use].start - address - 3 < 0)
                                {
                                    ta += 1 * (int)pow(16,3);
                                }
                            }
                            else
                            {
                                if(abs(symlitFind(littabHeader[key(srcOperand)], srcOperand)->loc - base->loc) < abs(symlitFind(littabHeader[key(srcOperand)], srcOperand)->loc - address - 3) && base->loc < address )
                                {
                                    ta += (symlitFind(littabHeader[key(srcOperand)], srcOperand)->loc - base->loc) % (int)pow(16,3);
                                    ta += 4 * (int)pow(16,3);
                                    if(symlitFind(littabHeader[key(srcOperand)], srcOperand)->loc - base->loc < 0)
                                    {
                                        ta += 1 * (int)pow(16,3);
                                    }
                                }
                                else
                                {
                                    ta += (symlitFind(littabHeader[key(srcOperand)], srcOperand)->loc - address - 3) % (int)pow(16,3);
                                    ta += 2 * (int)pow(16,3);
                                    if(symlitFind(littabHeader[key(srcOperand)], srcOperand)->loc - address - 3 < 0)
                                    {
                                        ta += 1 * (int)pow(16,3);
                                    }
                                }
                            }
                        }
                        else if(strcmp(ptr->info, "null") != 0)
                        {
                            if(base == NULL)
                            {
                                ta += (symlitFind(symtabHeader[key(srcOperand)], srcOperand)->loc + blockTab[symlitFind(symtabHeader[key(srcOperand)], srcOperand)->use].start - address - 3) % (int)pow(16,3) ;
                                ta += 2 * (int)pow(16,3);
                                if(symlitFind(symtabHeader[key(srcOperand)], srcOperand)->loc+ blockTab[symlitFind(symtabHeader[key(srcOperand)], srcOperand)->use].start - address - 3 < 0)
                                {
                                    ta += 1 * (int)pow(16,3);
                                }
                            }
                            else
                            {
                                if(abs(symlitFind(symtabHeader[key(srcOperand)], srcOperand)->loc - base->loc) < abs(symlitFind(symtabHeader[key(srcOperand)], srcOperand)->loc - address - 3) && base->loc < address )
                                {
                                    ta += (symlitFind(symtabHeader[key(srcOperand)], srcOperand)->loc - base->loc) % (int)pow(16,3) ;
                                    ta += 4 * (int)pow(16,3);
                                    if(symlitFind(symtabHeader[key(srcOperand)], srcOperand)->loc - base->loc < 0)
                                    {
                                        ta += 1 * (int)pow(16,3);
                                    }
                                }
                                else
                                {
                                    ta += (symlitFind(symtabHeader[key(srcOperand)], srcOperand)->loc - address - 3) % (int)pow(16,3) ;
                                    ta += 2 * (int)pow(16,3);
                                    if(symlitFind(symtabHeader[key(srcOperand)], srcOperand)->loc - address - 3 < 0)
                                    {
                                        ta += 1 * (int)pow(16,3);
                                    }
                                }
                            }
                        }
//                        printf("%s ta:%06X\n", srcCode, ta);
                        printf("%2d  %04X %d %s          %06X\n", row, address, use, srcStr, ta);
                        tPrint(fp_output, address, use, 3);
                        fprintf(fp_output, "%06X", ta);
                        continue;
                    }

                }
                else if(!strcmp(ptr->format, "2"))  //2
                {
                    ta += ptr->opcode * (int)pow(16,2);
                    for(i = 0; i < regtabHeaderMax; i++)
                    {
                        if(!strcmp(srcOperand, reg[i].name))
                        {
                            ta += reg[i].code * (int)pow(16,1);
                        }
                    }
                    if(srcOperand2 != NULL)
                    {
                        for(i = 0; i < regtabHeaderMax; i++)
                        {
                            if(!strcmp(srcOperand2, reg[i].name))
                            {
                                ta += reg[i].code;
                            }
                        }
                    }
                    printf("%2d  %04X %d %s          %04X\n", row, address, use, srcStr, ta);
                    tPrint(fp_output, address, use, 2);
                    fprintf(fp_output, "%04X", ta);
                    continue;
                }

            }
            else if(!strcmp(srcCode, "BASE"))
            {
                base = symlitFind(symtabHeader[key(srcOperand)], srcOperand);
            }
            else if(srcStr[srcExtendTagIndex + 1] == '=')
            {
                if(*srcCode == 'C')
                {
                    printf("%2d  %04X %d %s          ", row, address, use, srcStr);
                    for(i = 2; i < strlen(srcCode); i++)
                    {
                        if(*(srcCode + i) == '\'') break;
                        printf("%X", *(srcCode + i));
                        tPrint(fp_output, address, use, 1);
                        fprintf(fp_output, "%X",  *(srcCode + i));
                    }
                    printf("\n");
                    continue;
                }
                else if(*srcCode == 'X')
                {
                    printf("%2d  %04X %d %s          ", row, address, use, srcStr);
                    for(i = 2; i < strlen(srcCode); i++)
                    {
                        if(*(srcCode + i) == '\'') break;
                        printf("%X", *(srcCode + i) - 48);
                        tPrint(fp_output, address, use, 1);
                        fprintf(fp_output, "%X",  *(srcCode + i) - 48);
                    }
                    printf("\n");
                    continue;
                }
            }
            else if(!strcmp(srcCode, "BYTE"))
            {
                printf("%2d  %04X %d %s          ", row, address, use, srcStr);
                if(tptr->length == 0 || blockTab[use].start + address + 1 > tptr->start + tptr->length )
                {
                    tptr = tptr->next;
                    while(tptr->length == 0)
                    {
                        tptr = tptr->next;
                    }
                    fprintf(fp_output, "\nT%06X%02X", tptr->start, tptr->length);
                }
                for(i = 2; i < strlen(srcOperand); i++)
                {
                    if(*(srcOperand + i) == '\'') break;
                    printf("%C", *(srcOperand + i));
                    fprintf(fp_output, "%C", *(srcOperand + i));
                }
                printf("\n");
                continue;
            }
        }
        printf("%2d  %04X %d %s\n", row, address, use, srcStr);
    }
    mPrint(fp_output);
    fprintf(fp_output, "\nE%06X", blockTab[0].start);
    fclose(fp_input);
    fclose(fp_output);
    return 0;
}
//end
