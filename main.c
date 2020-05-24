#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define max 10
#define opcodeNameMax 7
#define inputMax 34

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

void print(optab_node * head) {
    optab_node * ptr;
    if(head->next == NULL) {
        printf("empty\n");
    }else{
        ptr = head->next;
        while(ptr != NULL) {
            printf("%6s %d %02X\n", ptr->name, ptr->format, ptr->opcode);
            ptr = ptr->next;
        }
    }
    printf("\n");
}

void create_Optab(void)
{
    optab_node* ptr;
    int i, sum = 0, readCol;
    for(i = 0; i < max; i ++)
    {
        optabHeader[i] = newNode();
    }
    FILE * fp_Optab = fopen("optab.txt", "r");
    int format, opcode;
    char name[opcodeNameMax];
    while(1) {
        fscanf(fp_Optab, "%s %d %x", name, &format, &opcode);
        if(feof(fp_Optab) != 0) break;
        for(i = 0; i < strlen(name); i++) {
            sum += name[i];
        }
        sum %= 10;
        ptr = optabHeader[sum];
        while(ptr->next != NULL) {
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
    for(i = 0; i < max; i++) {
        print(optabHeader[i]);
    }
}

int main()
{
    create_Optab();
    FILE * fp = fopen("srcpro.txt", "r");
    char inputStr[inputMax];
    while(1)
    {
        fgets(inputStr, inputMax, fp);
        if(feof(fp) != 0) break;

        printf("%s\n", inputStr);
    }

    fclose(fp);
    return 0;
}
