#include <stdio.h>
#include <stdlib.h>

struct optab_Node{
    char name[6];
    int format;
    int opcode;
    struct optab_Node * next;
};
typedef struct optab_Node optab_node;
optab_node optabHeader[10];

void create_Optab(optab_node* node) {
    FILE * fp_Optab = fopen("optab.txt");
    while(!feof(fp))
}

int main()
{
    FILE * fp;
    fp = fopen("srcpro.txt", "r");
    char str[20];
    while(1) {
        fscanf(fp, "%s", str);
        if(feof(fp) != 0) break;
        printf("%s\n", str);
    }

    fclose(fp);
    return 0;
}
