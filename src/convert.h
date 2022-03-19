//
// Created by 谢卫凯 on 2022/3/18.
//

#ifndef LAB1_CONVERT_H
#define LAB1_CONVERT_H
#include "DFA.h"
#include "NFA.h"
#include "bitarray.h"

//将NFA转化为DFA
DFA *toDFA(NFA *nfa);

//输出DFA信息  因为符号信息再nfa里面，所以nfa也作为参数
void ShowDFA(NFA *nfa,DFA *dfa,FILE *fp);

#endif //LAB1_CONVERT_H
