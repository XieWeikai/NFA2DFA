//
// Created by 谢卫凯 on 2022/3/17.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bitarray.h"
#include "NFA.h"
#include "DFA.h"
#include "convert.h"

#define COLOR_NONE "\033[0m"
#define RED "\033[1;31m"
#define BLUE "\033[1;34m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"

typedef struct{
    char *key;
    char *value;
}KV;

static KV *arguments[100] = {NULL};
static int argLen = 0;

void parseArg(int argc,char **argv){
    for(int i=1;i<argc;i++){
        if(argv[i][0] == '-'){
            arguments[argLen] = malloc(sizeof (KV));
            arguments[argLen]->key = argv[i];
            arguments[argLen++]->value = argv[++i];
        }
    }
}

char *getArg(char *s){
    for(int i=0;i<argLen;i++){
        if(strcmp(arguments[i]->key,s) == 0)
            return arguments[i]->value;
    }
    return NULL;
}

int main(int argc,char **argv){
    FILE *out = stdout,*in;
    char *filename;

    parseArg(argc,argv);
    if((filename = getArg("-i")) == NULL){
        fprintf(stderr,RED"error:"COLOR_NONE"no input file\n\n");
        printf("USAGE:NFA2DFA -i inputfile [-o outputfile]\nThe option -o is not necessary.\nDefault outputfile is stdout\n\n");
        exit(0);
    }
    if((in = fopen(filename,"r")) == NULL){
        fprintf(stderr,RED"error:"COLOR_NONE"no such input file "YELLOW"%s"COLOR_NONE".\n",filename);
        exit(-1);
    }
    if((filename = getArg("-o")) != NULL){
        out = fopen(filename,"w");
        if(out == NULL){
            fprintf(stderr,RED"error:"COLOR_NONE"no such output file "YELLOW"%s"COLOR_NONE".\n",filename);
            exit(-1);
        }
    }
    NFA *nfa = ParseNFA(in);
    DFA *dfa = toDFA(nfa);
    ShowDFA(nfa,dfa,out);


//    FILE *fp = fopen("/Users/admin/Desktop/学校相关/大二/形式语言与自动机/lab1/test2.nfa","r");
//    NFA *nfa = ParseNFA(fp);
//    showNFA(nfa);
//    printf("-------------\n");
//    DFA *dfa = toDFA(nfa);
//    ShowDFA(nfa,dfa,stdout);

//    Set *s = NewSet(),*s1 = NewSet();
//    printf("isEmpty:%d\n",isEmptySet(s));
//    Add(s,3);
//    Add(s,5);
//    Add(s,7);
//    Add(s,9);
//    showSet(s);
//    Add(s1,13);
//    Add(s1,90);
//    Add(s1,99);
//    Add(s1,98);
//    showSet(s1);
//    printf("same:%d\nintersection:%d\n",SameSet(s1,s),haveIntersection(s,s1));
    return 0;
}