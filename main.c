#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define max 10
#define opcodeNameMax 7
#define srcMax 33
#define srcTagMax 6
#define srcCodeMax 6
#define srcOperandMax 8

struct optab_Node
{
    char name[opcodeNameMax];
    int format;
    int opcode;
    struct optab_Node * next;
};
typedef struct optab_Node optab_node;
optab_node * optabHeader[max];

optab_node * newNode(void)
{
    optab_node * add = malloc(sizeof(optab_node));
    add->next = NULL;
    return add;
}

void print(optab_node * head)
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
            printf("%6s %d %02X\n", ptr->name, ptr->format, ptr->opcode);
            ptr = ptr->next;
        }
    }
    printf("\n");
}

void create_Optab(void)
{
    optab_node* ptr;
    int i, sum = 0;
    for(i = 0; i < max; i ++)
    {
        optabHeader[i] = newNode();
    }
    FILE * fp_Optab = fopen("optab.txt", "r");
    int format, opcode;
    char name[opcodeNameMax];
    while(1)
    {
        fscanf(fp_Optab, "%s %d %x", name, &format, &opcode);
        if(feof(fp_Optab) != 0) break;
        for(i = 0; i < strlen(name); i++)
        {
            sum += name[i];
        }
        sum %= 10;
        ptr = optabHeader[sum];
        while(ptr->next != NULL)
        {
            ptr = ptr->next;
        }
        ptr->next = newNode();
        ptr = ptr->next;
        strcpy(ptr->name, name);
        ptr->format = format;
        ptr->opcode = opcode;
        sum = 0;
    }
    fclose(fp_Optab);
    for(i = 0; i < max; i++)
    {
        print(optabHeader[i]);
    }
}

int main()
{
    create_Optab();
    FILE * fp_input = fopen("srcpro.txt", "r");
    FILE * fp_output = fopen("intermediate.txt", "w");
    char srcStr[srcMax], srcCode[srcCodeMax+1], srcOperand[srcOperandMax+1];
    int locctr;
    int flag = 0;
    while(fgets(srcStr, srcMax, fp_input) != NULL)
    {
        if(flag)
        {
            flag--;
            continue;
        }
        flag++;
        printf("%s1\n", srcStr);
        strncpy(srcCode, srcStr + srcTagMax + 2, srcCodeMax);
        srcCode[srcCodeMax] = '\0';
//        printf("%s\n", srcCode);
        if(strcmp(srcCode, "START ") == 0)
        {
            locctr = atoi(strncpy(srcOperand, srcStr + srcTagMax + 2 + srcCodeMax + 2, srcOperandMax));
            srcOperand[srcOperandMax] = '\0';
            fputs(srcStr, fp_output);
            continue;
//            printf("%d\n", locctr);
        }else{
            locctr = 0;
        }
        if(strcmp(srcCode, "END   ") != 0) {

//                if()

        }
    }

    fclose(fp_input);
    fclose(fp_output);
    return 0;
}
