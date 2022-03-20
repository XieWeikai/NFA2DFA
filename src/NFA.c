//
// Created by 谢卫凯 on 2022/3/17.
//

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "NFA.h"
#include "bitarray.h"

#define COLOR_NONE "\033[0m"
#define RED "\033[1;31m"
#define BLUE "\033[1;34m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"

#define STRING 0
#define STATES 1
#define START 2
#define FINISH 3
#define ALPHABET 4
//词法分析对应token状态

#define ISVALID(x) (((x)>='0' && (x)<='9') || ((x)>= 'a' && (x)<='z') || ((x)>= 'A' && (x)<='Z') || (x)=='_' )
//有效字母

void addEdg(NFA *nfa,int state,int alpha,Set *s);

static int token;
static char value[1024];
static int lineNum = 1;
//上为词法分析相关的变量


//查找符号的编号
int searchName(char **names,int size,char *name){
    for (int i=0;i<size;i++)
        if(strcmp(names[i],name) == 0)
            return i;
    return -1;
}

//跳过文件中的空格、换行、制表符
void skipBlank(FILE *fp){
    int ch;
    while((ch = getc(fp)) != EOF){
        if(ch == '\n') lineNum ++;
        if(ch != ' ' && ch != '\n' && ch != '\t'){
            //printf("skipBlank ungetc(%c)\n",ch);
            ungetc(ch,fp);
            return;
        }
    }
}

//如函数名
void nextToken(FILE *fp){
    int ch;
    char *p = value;
    skipBlank(fp);
    ch = getc(fp);
    if(!ISVALID(ch)){
        token = ch;
        return;
    }
    *p++ = ch;
    ch = getc(fp);
    while(ISVALID(ch)) {
        //printf("%c(%d)\n",ch,ch);
        *p++ = ch;
        ch = getc(fp);
    }
    ungetc(ch,fp);
    //printf("nextToken ungetc(%c)\n",ch);
    *p++ = 0;
    if(strcmp(value,"Q")==0)
        token = STATES;
    else if(strcmp(value,"T")==0)
        token = ALPHABET;
    else if(strcmp(value,"start")==0)
        token = START;
    else if(strcmp(value,"F")==0)
        token = FINISH;
    else
        token = STRING;
}

//辅助函数
void match(FILE *fp,int ch){
    if(token != ch){
        printf(RED"syntax error "YELLOW"line %d:"COLOR_NONE"expect '%c',get '%c'\n",lineNum,ch,token);
        exit(-1);
    }
    nextToken(fp);
}

//解析状态对应符号
void ParseStates(NFA *nfa,FILE *fp){
    char *sp;
    int pos = 0;
    match(fp,STATES);
    match(fp,'=');
    match(fp,'{');

    while(token == STRING){
        sp = malloc(strlen(value)+1);
        strcpy(sp,value);
        nfa->statesName[pos++] = sp;
        nextToken(fp);
        if(token == ',')
            match(fp,',');
    }

    if(pos == 0){
        printf(RED"error " YELLOW"Line %d:"COLOR_NONE"NFA have no state!\n",lineNum);
        exit(-1);
    }
    nfa->numOfStates = pos;

    nfa->states = malloc(pos * sizeof (State*));
    for (int i=0;i<pos;i++){
        nfa->states[i] = malloc(sizeof (State));
        nfa->states[i]->type = STATE;
        nfa->states[i]->edgs = NULL;
    }

    match(fp,'}');
}

//解析字母表对应符号
//epsilon是内置好的
void ParseAlphabet(NFA *nfa,FILE *fp){
    char *sp;
    int pos = 1;
    nfa->alphaName[0] = "epsilon";
    match(fp,ALPHABET);
    match(fp,'=');
    match(fp,'{');

    while(token == STRING){
        sp = malloc(strlen(value)+1);
        strcpy(sp,value);
        nfa->alphaName[pos++] = sp;
        nextToken(fp);
        if(token == ',')
            match(fp,',');
    }
    nfa->numOfAlpha = pos;
    match(fp,'}');
}

//解析起始符对应符号
void ParseStartState(NFA *nfa,FILE *fp){
    match(fp,START);
    match(fp,'=');
    if(token != STRING){
        printf(RED"error "YELLOW"line %d:"COLOR_NONE"can not get start state\n",lineNum);
        exit(-1);
    }
    nfa->startInd = searchName(nfa->statesName,nfa->numOfStates,value);
    if(nfa->startInd == -1){
        printf(RED"error "YELLOW"line %d:"COLOR_NONE"no such state name(%s)\n",lineNum,value);
        exit(-1);
    }
    nextToken(fp);
}

//解析终止符符号
void ParseFState(NFA *nfa,FILE *fp){
    int n;
    match(fp,FINISH);
    match(fp,'=');
    match(fp,'{');

    while(token == STRING){
        n = searchName(nfa->statesName,nfa->numOfStates,value);
        if(n == -1){
            printf(RED"error "YELLOW"line %d:"COLOR_NONE"no such state name(%s)\n",lineNum,value);
            exit(-1);
        }
        nfa->states[n]->type = ENDSTATE;
        nextToken(fp);
        if(token == ',')
            nextToken(fp);
    }

    match(fp,'}');
}

//纯属SB的辅助函数
int handleSearchError(NFA *nfa,char *pattern,char *name){
    int res;
    if(strcmp(pattern,"state") == 0){
        res = searchName(nfa->statesName,nfa->numOfStates,name);
        if(res == -1){
            printf(RED"error "YELLOW"line %d:"COLOR_NONE"no such state name(%s)\n",lineNum,value);
            exit(-1);
        }
        return res;
    }else{
        res = searchName(nfa->alphaName,nfa->numOfAlpha,name);
        if(res == -1){
            printf(RED"error "YELLOW"line %d:"COLOR_NONE"no such alphabet name(%s)\n",lineNum,value);
            exit(-1);
        }
        return res;
    }
}

//解析一条状态转换规则
void ParseItem(NFA *nfa,FILE *fp){
    int state,alpha,next;
    Set *nextStates = NewSet();
    if(token != STRING){
        printf(RED"error "YELLOW"line %d:"COLOR_NONE"exect state name\n",lineNum);
        exit(-1);
    }
    state = handleSearchError(nfa,"state",value);
    //printf("get state %d\n",state);
    nextToken(fp);
    match(fp,',');
    if(token != STRING){
        printf(RED"error "YELLOW"line %d:"COLOR_NONE"exect alphabet name\n",lineNum);
        exit(-1);
    }
    alpha = handleSearchError(nfa,"alphabet",value);
    nextToken(fp);
    match(fp,'=');
    match(fp,'>');
    match(fp,'{');

    while(token == STRING){
        next = handleSearchError(nfa,"state",value);
        Add(nextStates,next);
        nextToken(fp);
        if(token == ',')
            nextToken(fp);
    }
    //printf("add state %d\n\n",state);
    addEdg(nfa,state,alpha,nextStates);

    match(fp,'}');
}

//解析整个NFA
NFA *ParseNFA(FILE *fp){
    NFA *nfa = malloc(sizeof (*nfa));
    nextToken(fp);
    ParseStates(nfa,fp);

    int alph = 0,start = 0,FStat = 0;
    while(!(alph && start && FStat)){
        if(token == ALPHABET){
            if(alph == 1){
                fprintf(stderr,"error:"COLOR_NONE"duplicated alphabet\n");
                exit(-1);
            }
            ParseAlphabet(nfa,fp);
            alph = 1;
        }
        if(token == START){
            if(start == 1){
                fprintf(stderr,"error:"COLOR_NONE"duplicated start state definition\n");
                exit(-1);
            }
            ParseStartState(nfa,fp);
            start = 1;
        }
        if(token == FINISH){
            if(FStat == 1){
                fprintf(stderr,"error:"COLOR_NONE"duplicated F state definition\n");
                exit(-1);
            }
            ParseFState(nfa,fp);
            FStat = 1;
        }
    }
//    ParseAlphabet(nfa,fp);
//    ParseStartState(nfa,fp);
//    ParseFState(nfa,fp);

    while(token != EOF)
        ParseItem(nfa,fp);
    return nfa;
}

//debug用的
//展示NFA结构和符号
void showNFA(NFA *nfa){
    char *s[] = {"STATE","ENDSTATE"};
    for(int i=0;i<nfa->numOfStates;i++){
        printf("%s(%s) edgs:",nfa->statesName[i],s[nfa->states[i]->type]);
        for(Node *t=nfa->states[i]->edgs;t != NULL;t=t->next) {
            printf(GREEN"%s:"COLOR_NONE, nfa->alphaName[t->k]);
            Iter it = NewIter(t->nextStats);
            printf(RED);
            for(int st=Next(it);st != -1;st = Next(it))
                printf("%s ",nfa->statesName[st]);
            printf(COLOR_NONE);
            printf("->");
            destroyIter(it);
        }
        printf("NULL\n");
    }
    printf("start:%s\n",nfa->statesName[nfa->startInd]);
    printf("alphabet:");
    for(int i=0;i<nfa->numOfAlpha;i++)
        printf("%d:%s ",i,nfa->alphaName[i]);
    printf("\n");
    printf("states:");
    for(int i=0;i<nfa->numOfStates;i++)
        printf("%d:%s ",i,nfa->statesName[i]);
    printf("\n");
}

//debug
void test(FILE *fp){
    printf("start testing\n");
    char *s[] = {"STRING","STATES","START","FINISH","ALPHABET"};
    nextToken(fp);
    char *sp;
    while(token != EOF){
        if(token >= 5)
            sp = "CHAR";
        else
            sp = s[token];
        printf("token:%c %s   value:%s\n",token,sp,value);
        nextToken(fp);
    }
    printf("end of testing\n");
}

//链表插入 头插
List add(List l,Node *n){
    n->next = l;
    return n;
}

//链表新节点
Node *NewNode(Set *s,int k){
    Node *t = malloc(sizeof (*t));
    t->next = NULL;
    t->k = k;
    t->nextStats = s;
    return t;
}

//加入一条边 NFA 中state经过字母alpha到达s状态集合
void addEdg(NFA *nfa,int state,int alpha,Set *s){
    Node *t;
    for(t = nfa->states[state]->edgs;t!=NULL && t->k != alpha;t = t->next)
        ;
    if(t != NULL){//重复定义了一个转换规则，报错
        printf(RED"error:"COLOR_NONE"duplicated defination of δ(%s,%s)\n",nfa->statesName[state],nfa->alphaName[alpha]);
        exit(-1);
    }
    t = NewNode(s,alpha);
    nfa->states[state]->edgs = add(nfa->states[state]->edgs,t);
}

//返回state 经过k到达的状态集合 没有则返回NULL
Set *getNextStates(NFA *nfa,int state,int k){
    Node *t;
    for(t = nfa->states[state]->edgs;t!=NULL && t->k != k;t=t->next)
        ;
    if(t != NULL)
        return t->nextStats;
    return NULL;
}
