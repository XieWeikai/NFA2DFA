//
// Created by 谢卫凯 on 2022/3/17.
// 提供了解析文件为NFA结构的功能
//

#ifndef LAB1_NFA_H
#define LAB1_NFA_H

#define ENDSTATE 1
#define STATE 0

#include "bitarray.h"

typedef struct node{
    int k; //字母  字母表每个字母对应一个整数
    Set *nextStats; //下一个状态，是一个集合
    struct node *next; //下一个字母
}Node,*List;

typedef struct {
    int type; //是否为终止状态
    List edgs;  //状态转移
}State;

typedef struct {
    State **states;
    int startInd; //起始状态
    char *statesName[1024]; //states[ind]对应的名字就是statesName[ind]
    char *alphaName[1024]; //字母表名字 alphaName[0]一定是"epsilon"
    int numOfStates;  //状态个数
    int numOfAlpha;  //字母个数  该程序将epsilon看做一个字母
}NFA;

//从一个文件解析出对应的NFA
NFA *ParseNFA(FILE *fp);

//展示该NFA对应的结构和符号信息
void showNFA(NFA *nfa);

//返回nfa的状态state经过字母k后到达的状态集合
Set *getNextStates(NFA *nfa,int state,int k);

#endif //LAB1_NFA_H
