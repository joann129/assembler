#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define headerMax 10
#define opcodeNameMax 7
#define opcodeFormatyMax 4
#define opcodeInfoMax 6
#define symtabNameMax 9
#define srcMax 33
#define srcTagMax 6
#define srcCodeMax 6
#define srcOperandMax 8
#define srcExtendTagIndex 7
#define srcOperTagIndex 15
#define srcOperatorIndex 24

int locctr = 0, startLoc;
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

struct symtab_Node
{
    char name[symtabNameMax];
    int loc;
    int addressFlag;
    struct symtab_Node * next;
};
typedef struct symtab_Node symtab_node;
symtab_node * symtabHeader[headerMax];
symtab_node * littabHeader[headerMax];

int key(char name[opcodeNameMax])
{
    int i, sum = 0;
    for(i = 0; i < strlen(name); i++)
    {
//        printf("%d ", name[i]);
        sum += name[i];
    }
    sum %= 10;
//    printf("\n");
    return sum;

}

optab_node * optabNewNode(void)
{
    optab_node * add = malloc(sizeof(optab_node));
    add->next = NULL;
    return add;
}

symtab_node * symlitNewNode(void)
{
    symtab_node * add = malloc(sizeof(symtab_node));
    add->addressFlag = 0;
    add->next = NULL;
    return add;
}

void optabPrint(optab_node * head)
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
            printf("%6s %3s %02X %5s\n", ptr->name, ptr->format, ptr->opcode, ptr->info);
            ptr = ptr->next;
        }
    }
    printf("\n");
}

void optabCreate(void)
{
    optab_node* ptr;
    int i;
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
    for(i = 0; i < headerMax; i++)
    {
        optabPrint(optabHeader[i]);
    }
}

void symlitPrint(symtab_node * head)
{
    symtab_node * ptr;
    if(head->next == NULL)
    {
        printf("empty\n");
    }
    else
    {
        ptr = head->next;
        while(ptr != NULL)
        {
            printf("%6s %04X\n", ptr->name, ptr->loc);
            ptr = ptr->next;
        }
    }
    printf("\n");
}

void symlitInsert(symtab_node * head, char name[symtabNameMax], int loc)
{
    symtab_node * ptr;
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

symtab_node * symlitFind(symtab_node * head, char tag[symtabNameMax])
{
    symtab_node * ptr = head;
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

void littabAddressing(FILE* fp, symtab_node * head)
{
    symtab_node * ptr;
    if(head->next == NULL) {
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
            ptr->loc = locctr;
            printf("%04X *      =%s\n", ptr->loc, ptr->name);
            fprintf(fp, "%04X *      =%s\n", ptr->loc, ptr->name);
            if(ptr->name[0] == 'X')
            {
                locctr += (strlen(ptr->name) - 3) / 2;
            }
            else if(ptr->name[0] == 'C')
            {
                locctr += strlen(ptr->name) - 3;
            }
            ptr->loc = locctr;

        }
        ptr= ptr->next;
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
//        printf("%s %d\n", tok, strlen(tok));
        return tok;
    }
}

int main()
{
    optabCreate();
    symtabCreate();
    littabCreate();
    FILE * fp_input = fopen("srcpro.txt", "r");
    FILE * fp_output = fopen("intermediate.txt", "w");
    char srcStr[srcMax], temp[10], temp1[10], temp2[10], temp3[10];
    char *srcTag, *srcCode, *srcOperand, *srcOperand2;
    int flag = 0, srcOper, keyTemp, i, value;
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
            locctr = srcOper;
            startLoc = locctr;
            fprintf(fp_output, "%04X %s\n", locctr, srcStr);
            continue;
        }
//        else
//        {
//            locctr = 0;
//        }
        if(strcmp(srcCode, "END") != 0)
        {
            if(srcTag != NULL)
            {
                keyTemp = key(srcTag);
//                printf("key is %d\n", keyTemp);
                if(symlitFind(symtabHeader[keyTemp], srcTag) == NULL)
                {
                    if(!strcmp(srcCode, "EQU"))
                    {
                        if(srcStr[srcOperatorIndex] == ' ')
                        {
                            if(!strcmp(srcOperand, "*"))
                            {
                                value = locctr;
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
                        symlitInsert(symtabHeader[keyTemp], srcTag, locctr);
                    }

                }
                else
                {
                    printf("輸入檔有重複符號\n");
                }
            }

            if(!strcmp(srcCode, "EQU"))
            {
                printf("%04X %33s \n", value, srcStr);
                fprintf(fp_output, "%04X %s \n", value, srcStr);
            }
            else
            {
                printf("%04X %s \n", locctr, srcStr);
                fprintf(fp_output, "%04X %s \n", locctr, srcStr);
            }


//            printf("%X\n", locctr);
            if(!strcmp(srcCode, "WORD"))
            {
                locctr += 3;
            }
            else if(!strcmp(srcCode, "RESW"))
            {
                locctr += 3 * srcOper;
            }
            else if(!strcmp(srcCode, "RESB"))
            {
                locctr += srcOper;
            }
            else if(!strcmp(srcCode, "BYTE"))
            {
                if(*srcOperand == 'X')
                {
                    locctr += (strlen(srcOperand) - 3) / 2;
                }
                else if(*srcOperand == 'C')
                {
                    locctr += strlen(srcOperand) - 3;
                }

            }
            else if(srcStr[srcExtendTagIndex] == '+')
            {
                locctr += 4;
            }
            else if(!strcmp(srcCode, "EQU"))
            {

            }
            else if(!strcmp(srcCode, "LTORG"))
            {
                for(i = 0; i < headerMax; i++)
                {
                    littabAddressing(fp_output, littabHeader[i]);
                }
            }
            else
            {
                ptr = optabHeader[key(srcCode)];
                while(ptr != NULL)
                {
//                    printf("%s %s\n", ptr->name, srcCode);
                    if(!strcmp(ptr->name, srcCode))
                    {
                        if(!strcmp(ptr->format, "1"))
                        {
                            locctr += 1;
                        }
                        else if(!strcmp(ptr->format, "2"))
                        {
                            locctr += 2;
                        }
                        else
                        {
                            locctr += 3;
                        }

                        break;
                    }
                    else
                    {
                        ptr = ptr->next;
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
    }//while
    for(i = 0; i < 10; i++)
    {
        symlitPrint(symtabHeader[i]);
    }
    for(i = 0; i < 10; i++)
    {
        symlitPrint(littabHeader[i]);
    }
    fclose(fp_input);
    fclose(fp_output);
    return 0;
}
