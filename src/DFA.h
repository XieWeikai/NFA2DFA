//
// Created by 谢卫凯 on 2022/3/18.
//

#ifndef LAB1_DFA_H
#define LAB1_DFA_H

#include "bitarray.h"

#define DFASTART 0
#define DFAEND 1
#define DFASTATE 2
#define DFASTARTENDSTATE 3

typedef struct dfanode{
    int k; //字母
    int nextState; //下一个状态
    struct dfanode *next;
}DFANode,*DFAList;

typedef struct{
    Set *NFAStates;
    int type;
    DFAList edgs;
}DFAState;

typedef struct {
    DFAState **states;
    int len,cap;
}DFA;

//初始化一个DFA 相当于一个空图 什么都没有 只是先占了些内存
DFA *NewDFA();

//放入一个状态 hfaS为该DFA状态对应的HFA状态
//返回放入状态对应的编号
int addDFAState(DFA *dfa, Set *hfaS,int type);

//s为NFA状态对应的集合
//一个DFA状态相当于NFA状态对应的集合
//查找DFA中是否有对应的状态
int searchDFAState(DFA *dfa,Set *s);

//从DFA中的状态state经过字母alpha到达的状态为nextState
void addEge(DFA *dfa,int state,int alpha,int nextState);

#endif //LAB1_DFA_H
