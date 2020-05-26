#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#define headerMax 10
#define opcodeNameMax 7
#define opcodeFormatyMax 4
#define symtabNameMax 7
#define srcMax 33
#define srcTagMax 6
#define srcCodeMax 6
#define srcOperandMax 8

struct optab_Node
{
    char name[opcodeNameMax];
    char format[opcodeFormatyMax];
    int opcode;
    struct optab_Node * next;
};
typedef struct optab_Node optab_node;
optab_node * optabHeader[headerMax];

struct symtab_Node{
    char name[symtabNameMax];
    int loc;
    struct symtab_Node * next;
};
typedef struct symtab_Node symtab_node;
symtab_node * symtabHeader[headerMax];


char *trim(char *str)
{
	if (str == NULL || *str == '\0')
	{
		return str;
	}
	int len = strlen(str);
	char *p = str + len - 1;
	while (p >= str  && isspace(*p))
	{
		*p = '\0';
		--p;
	}

	return str;
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

optab_node * optabNewNode(void)
{
    optab_node * add = malloc(sizeof(optab_node));
    add->next = NULL;
    return add;
}

symtab_node * symtabNewNode(void) {
    symtab_node * add = malloc(sizeof(symtab_node));
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
            printf("%6s %3s %02X\n", ptr->name, ptr->format, ptr->opcode);
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
    char name[opcodeNameMax], format[opcodeFormatyMax];
    while(1)
    {
        fscanf(fp_Optab, "%s %s %x", name, format, &opcode);
        if(feof(fp_Optab) != 0) break;
//        for(i = 0; i < strlen(name); i++)
//        {
//            sum += name[i];
//        }
//        sum %= 10;
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
    }
    fclose(fp_Optab);
    for(i = 0; i < headerMax; i++)
    {
        optabPrint(optabHeader[i]);
    }
}

void symtabPrint(symtab_node * head)
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
            printf("%6s %04d\n", ptr->name, ptr->loc);
            ptr = ptr->next;
        }
    }
    printf("\n");
}

void symtabInsert(int index, char name[symtabNameMax], int loc) {
    symtab_node * ptr;
    ptr = symtabHeader[index];
    while(ptr->next != NULL) {
        ptr = ptr->next;
    }
    ptr->next = symtabNewNode();
    ptr = ptr->next;
    strcpy(ptr->name, name);
    ptr->loc = loc;
}

int symtabFind(int index, char tag[srcTagMax]) {
    symtab_node * ptr = symtabHeader[index];
    while(ptr->next != NULL) {
        if(strncmp(ptr->name, tag, 6) == 0) {
            return 1;
        }
        ptr = ptr->next;
    }
    return 0;
}

void symtabCreate(void) {
    int i;
    for(i = 0; i < headerMax; i++) {
        symtabHeader[i] = symtabNewNode();
    }
}

//int decemalToHex(int num) {
//    int i, j, ans = 0;
//    for(i = 0; i < 4; i++) {
//        ans += (num % (int)pow(16, i)) * (int)pow(10, i);
//    }
//    return ans;
//}

int main()
{
    optabCreate();
    symtabCreate();
    FILE * fp_input = fopen("srcpro.txt", "r");
    FILE * fp_output = fopen("intermediate.txt", "w");
    char srcStr[srcMax], srcTag[srcTagMax], srcCode[srcCodeMax+1], srcOperand[srcOperandMax+1];
    int locctr, startLoc;
    int flag = 0, srcOper, keyTemp;
    optab_node * ptr;
    while(fgets(srcStr, srcMax, fp_input) != NULL)
    {
        if(flag)
        {
            flag--;
            continue;
        }
        flag++;
        printf("%s\n", srcStr);
        strncpy(srcTag, srcStr, srcTagMax);
        strncpy(srcCode, srcStr + srcTagMax + 2, srcCodeMax);
        strncpy(srcOperand, srcStr + srcTagMax + 2 + srcCodeMax + 2, srcOperandMax);
        srcOper = atoi(srcOperand);
//        printf("%s\n", srcCode);
        if(strncmp(srcCode, "START", 5) == 0)
        {
            locctr = srcOper;
            startLoc = locctr;
            fputs(srcStr, fp_output);
            fputs("\n", fp_output);
            continue;
//            printf("%d\n", locctr);
        }
        else
        {
            locctr = 0;
        }
        if(strncmp(srcCode, "END", 3) != 0)
        {

//            for(i = 0; i < strlen(srcCode); i++)
//            {
//                sum += srcCode[i];
//            }
//            sum %= 10;

//            keyTemp = key(srcTag);
//            if(!symtabFind(keyTemp, srcTag)) {
//                symtabInsert(keyTemp, srcTag, locctr);
//            }else{
//                printf("輸入檔有重複符號\n");
//            }
//            symtabPrint(symtabHeader[keyTemp]);
            ptr = optabHeader[key(srcCode)];
            if(strncmp(srcCode, "WORD", 4) == 0)
            {
                locctr += 3;
            }
            else if(strncmp(srcCode, "RESW", 4) == 0)
            {
                locctr += 3 * srcOper;
            }
            else if(strncmp(srcCode, "RESB", 4) == 0)
            {
                locctr += srcOper;
            }
            else if(strncmp(srcCode, "BYTE", 4) == 0)
            {

            }
            while(ptr->next != NULL)
            {
                if(strncmp(ptr->name, srcCode, strlen(ptr->name)) == 0)
                {
                    locctr += 3;
                    break;
                }
                else
                {
                    ptr = ptr->next;
                }
            }

        }
    }

    fclose(fp_input);
    fclose(fp_output);
    return 0;
}
