//
// Created by 谢卫凯 on 2022/3/18.
//

#include <stdio.h>
#include <stdlib.h>

#include "convert.h"
#include "bitarray.h"
#include "DFA.h"
#include "NFA.h"

#define DEBUG 0

//链表队列节点
typedef struct nodeint{
    int data;
    struct nodeint *next;
}NodeInt;

//队列结构
typedef struct {
    NodeInt *head;
    NodeInt *tail;
}*Queue;

//新建队列
Queue newQueue(){
    Queue q = malloc(sizeof (*q));
    q->head = q->tail = malloc(sizeof (NodeInt));
    q->head->next = NULL;
    return q;
}

//销毁队列
void DestroyQueue(Queue q){
    NodeInt *p;
    for(p=q->head,q->head=q->head->next;q->head!=NULL;q->head=q->head,q->head=q->head->next)
        free(p);
    free(p);
    free(q);
}

//入队
void enque(Queue q,int e){
    NodeInt *n = malloc(sizeof (*n));
    n->data = e;
    n->next = NULL;
    q->tail->next = n;
    q->tail = n;
}

//队列是否为空
int isEmpty(Queue q){
    return q->head == q->tail;
}

//出队
int deque(Queue q){
    NodeInt *t;
    int res;
    if(q->head == q->tail)
        return -1;
    t = q->head->next;
    q->head->next = t->next;
    res = t->data;
    if(t->next == NULL)
        q->tail = q->head;
    return res;
}

//深度优先搜索，将通过epsilon边能到达的位置都走一遍
void dfs(NFA *nfa,int state,Set *clo){
    Node *t;
    Iter it;
    int next;
    Add(clo,state);
    for(t = nfa->states[state]->edgs;t != NULL && t->k != 0;t=t->next)
        ;
    if(t == NULL)
        return;
    it = NewIter(t->nextStats);
    for(next=Next(it);next != -1;next = Next(it))
        if(!In(clo,next))
            dfs(nfa,next,clo);
    destroyIter(it);
}

//求一个状态的epsilon闭包 返回状态集合
Set *closure(NFA *nfa,int state){
    Set *clo = NewSet();

    dfs(nfa,state,clo);
    return clo;
}

//nfa转dfa
DFA *toDFA(NFA *nfa){
    Set *mySet,*endState = NewSet(),*tmpS=NULL,*tmp;
    int state,tmpState,searchState;
    DFA *dfa = NewDFA();
    Queue que = newQueue();
    Iter it,itt;

    for(int i=0;i<nfa->numOfStates;i++)
        if(nfa->states[i]->type == ENDSTATE)
            Add(endState,i);

    //printf("end state set :");showSet(endState);printf("\n");

    mySet = closure(nfa,nfa->startInd); //初始状态为nfa初始状态的闭包
    //新状态入队，接下来要通过新状态来搜索更新的状态
    if(haveIntersection(mySet,endState))
        state = addDFAState(dfa,mySet,DFASTARTENDSTATE);
    else
        state = addDFAState(dfa,mySet,DFASTART);
    enque(que,state);
    while(!isEmpty(que)){
        state = deque(que);
        for(int k=1;k<nfa->numOfAlpha;k++){//计算每一个字母对应的下一个状态
            mySet = NewSet();
            it = NewIter(dfa->states[state]->NFAStates); //it迭代这个DFA状态对应的所有NFA状态
#if DEBUG
            printf("now the state is ");showSet(dfa->states[state]->NFAStates);printf("\n");
#endif
            for(int st=Next(it);st != -1;st=Next(it)) { //对于每个NFA状态
                tmpS = getNextStates(nfa, st, k); //这个字母的下一些状态
                //printf("show tmpS:");showSet(tmpS);printf("\n");
                if (tmpS == NULL)
                    continue;
                itt = NewIter(tmpS); //对于这些状态要计算闭包才行
#if DEBUG
                printf("for state %d after:%s can reach:",st,nfa->alphaName[k]);showSet(tmpS);printf("\n");
#endif
                for (int i = Next(itt); i != -1; i = Next(itt)) {//计算闭包
                    tmp = closure(nfa, i);
#if DEBUG
                    printf("closure of %d:",i);showSet(tmp);printf("\n");
#endif
                    Union(mySet, tmp);
                    destroy(tmp);
                }
                destroyIter(itt);
                //printf("after state:%s:",nfa->statesName[st]);showSet(mySet);printf("\n");
            }
#if DEBUG
            printf("after alpha %s ",nfa->alphaName[k]);showSet(mySet);printf("\n\n");
#endif
            destroyIter(it);
            if(!isEmptySet(mySet)){ //mySet对应到达的状态
                searchState = searchDFAState(dfa,mySet); //看看到达的状态是否是新状态
                if(searchState == -1){ //新状态
                    if(haveIntersection(mySet,endState))//和NFA终止符有交集 是终止状态
                        tmpState = addDFAState(dfa,mySet,DFAEND);
                    else
                        tmpState = addDFAState(dfa,mySet,DFASTATE);
                    enque(que,tmpState); //新状态入队
                    addEge(dfa,state,k,tmpState); //加入状态转移的边
                }else{
                    addEge(dfa,state,k,searchState); //不是新状态，加入状态转移的边即可
                    destroy(mySet); //防止内存泄露
                }
            }else{destroy(mySet);} //空集 释放掉
        }
    }
    DestroyQueue(que); //销毁队列
    return dfa;
}

//输出dfa 对应的nfa中存有符号信息，也得传入
void ShowDFA(NFA *nfa,DFA *dfa,FILE *fp){
    fprintf(fp,"Q = {");
    for(int i=0;i<dfa->len-1;i++)
        fprintf(fp,"Q%d,",i);
    fprintf(fp,"Q%d}\n\n",dfa->len-1);
    fprintf(fp,"T = {");
    for(int i=1;i<nfa->numOfAlpha-1;i++)
        fprintf(fp,"%s,",nfa->alphaName[i]);
    fprintf(fp,"%s}\n\n",nfa->alphaName[nfa->numOfAlpha-1]);
    fprintf(fp,"start = Q0\n\n");
    fprintf(fp,"F = { ");
    for(int i=0;i<dfa->len;i++){
        if(dfa->states[i]->type == DFAEND || dfa->states[i]->type == DFASTARTENDSTATE)
            fprintf(fp,"Q%d ",i);
    }
    fprintf(fp,"}\n\n");

    for(int i=0;i<dfa->len;i++){
        if(dfa->states[i]->edgs != NULL){ //有后继状态
            for(DFANode *t=dfa->states[i]->edgs;t != NULL;t = t->next)
                fprintf(fp,"Q%d,%s => Q%d\n",i,nfa->alphaName[t->k],t->nextState);
        }
        fprintf(fp,"\n");
    }
    fprintf(fp,"----------------\n");
    for(int i=0;i<dfa->len;i++){
        Iter it = NewIter(dfa->states[i]->NFAStates);
        fprintf(fp,"Q%d = [",i);
        for(int t=Next(it);t != -1;t = Next(it))
            fprintf(fp,"%s ",nfa->statesName[t]);
        fprintf(fp,"]\n");
        destroyIter(it);
    }
}

//debug
void test_convert(NFA *nfa,int state){
    printf("start test\n");
    Set *clo = closure(nfa,state);
    Iter it = NewIter(clo);
    for(int n=Next(it);n!=-1;n=Next(it))
        printf("%d ",n);
    printf("\n");
    printf("end test\n");
    Queue q = newQueue();
    enque(q,1);
    enque(q,2);
    enque(q,3);
    enque(q,4);
    printf("%d %d ",deque(q),deque(q));
    enque(q,5);
    printf("%d %d %d\n",deque(q),deque(q),deque(q));

    DestroyQueue(q);
}