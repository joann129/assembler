#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define max 10
#define opcodeNameMax 7

struct optab_Node
{
    char name[opcodeNameMax];
    int format;
    int opcode;
    struct optab_Node * next;
};
typedef struct optab_Node optab_node;
optab_node * optabHeader[10];

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
            printf("%s\n", ptr->name);
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
    char str_op[opcodeNameMax];
    while(1) {
        fscanf(fp_Optab, "%s", str_op);
        if(feof(fp_Optab) != 0) break;
        for(i = 0; i < strlen(str_op); i++) {
            sum += str_op[i];
        }
        sum %= 10;
        ptr = optabHeader[sum];
        while(ptr->next != NULL) {
            ptr = ptr->next;
        }
        ptr->next = newNode();
        ptr = ptr->next;
        strcpy(ptr->name, str_op);
        fscanf(fp_Optab, "%s", str_op);
        //ptr->format = atoi(str_op);
        fscanf(fp_Optab, "%s", str_op);
        ptr->opcode = atoi(str_op);
        sum = 0;
    }
    fclose(fp_Optab);
    for(i = 0; i < max; i++) {
        print(optabHeader[i]);
    }
    //print
}

int main()
{
    create_Optab();
    FILE * fp = fopen("srcpro.txt", "r");
    char str[20];
    while(1)
    {
        fscanf(fp, "%s", str);
        if(feof(fp) != 0) break;
        printf("%s\n", str);
    }

    fclose(fp);
    return 0;
}
