//
// Created by 谢卫凯 on 2022/3/18.
//
#include <stdio.h>
#include <stdlib.h>

#include "DFA.h"
#include "bitarray.h"

DFA *NewDFA(){
    DFA *dfa = malloc(sizeof (*dfa));
    dfa->states = malloc(1024 * sizeof (DFAState*));
    dfa->len = 0;
    dfa->cap = 1024;
    return dfa;
}

int addDFAState(DFA *dfa, Set *hfaS,int type){
    DFAState *state = malloc(sizeof (*state));
    state->type = type;
    state->edgs = NULL;
    state->NFAStates = hfaS;
    if(dfa->len >= dfa->cap) {
        dfa->states = realloc(dfa->states, dfa->cap * 2 * sizeof(DFAState *));
        dfa->cap*=2;
    }
    dfa->states[dfa->len] = state;
//    char *s[] = {"DFASTART","DFAEND","DFASTATE"};
//    printf("[%d]addDFAState:",dfa->len);showSet(hfaS);printf("type:%s\n",s[type]);

    return dfa->len++;
}

int searchDFAState(DFA *dfa,Set *s){
    for(int i=0;i<dfa->len;i++)
        if(SameSet(dfa->states[i]->NFAStates,s))
            return i;
    return -1;
}

void addEge(DFA *dfa,int state,int alpha,int nextState){
    DFANode *t = malloc(sizeof (*t));
    t->k = alpha;
    t->nextState = nextState;
    t->next = dfa->states[state]->edgs;
    dfa->states[state]->edgs = t;
}
