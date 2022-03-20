//
// Created by 谢卫凯 on 2022/3/17.
//
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "bitarray.h"

#define DEFAULTSIZE 8

#define INTSIZE sizeof(int)

Set *NewSet(){
    Set *t = calloc(1,sizeof (*t));
    t->m = calloc(DEFAULTSIZE,sizeof (unsigned  int)); //初始化为0
    t->top = -1;
    t->size = DEFAULTSIZE;
    return t;
}

void Add(Set *s,int e){
    int i,b;
    i = e >> 5;
    b = e & 0x1f;
    while(i >= s->size){
        s->m = realloc(s->m,s->size*2*sizeof (unsigned int));
        for(int t = s->size;t < s->size*2;t++)
            s->m[t] = 0; //初始化为0
        s->size >>= 1;
    }
    if(i > s->top)
        s->top = i;
    s->m[i] |= (1 << b);
}

void Remove(Set *s,int e){
    int i,b;
    i = e >> 5;
    b = e & 0x1f;
    s->m[i] &= ~(1<<b);
    if(s->m[s->top] == 0){
        for(i=s->top-1;s->m[i]==0 && i>=0;i--)
            ;
        s->top = i >= 0?i:0;
    }
    if(s->top < (s->size>>2)){
        s->m = realloc(s->m,s->size>>1);
        s->size >>= 1;
    }
}

int In(Set *s,int e){
    int i,b;
    i = e >> 5;
    b = e & 0x1f;
    if(i > s->top)
        return 0;
    return (s->m[i] & (1<<b)) != 0;
}

void Union(Set *s1,Set *s2){
    if(s1->top < s2->top){
        if(s1->size <= s2->top)
            s1->m = realloc(s1->m,s2->size);
        s1->top = s2->top;
    }

    int min = s1->top <= s2->top ? s1->top:s2->top;
    for(int i=0;i<=min;i++)
        s1->m[i] |= s2->m[i];
}

void destroy(Set *s){
    free(s->m);
    free(s);
}

Iter NewIter(Set *s){
    int cur = 0,i,b;
    Iter it = malloc(sizeof (*it));
    it->s = s;
    while(1){
        i = cur >> 5;
        b = cur & 0x1f;
        if(i > s->top){
            it->cur = -1;
            return it;
        }
        if((s->m[i] & (1<<b)) != 0){
            it->cur = cur;
            return it;
        }
        cur ++;
    }
}

int Next(Iter it){
    int res = it->cur;
    int cur = it->cur+1;
    int i,b;
    if(it->cur == -1)
        return -1;
    while(1){
        i = cur >> 5;
        b = cur & 0x1f;
        if(i > it->s->top){
            it->cur = -1;
            break;
        }
        if((it->s->m[i] & (1<<b)) != 0){
            it->cur = cur;
            break;
        }
        cur ++;
    }
    return res;
}

//destroy an iterator
void destroyIter(Iter it){
    free(it);
}

int SameSet(Set *s1, Set *s2) {
    if(s1->top != s2->top)
        return 0;
    for(int i=0;i<=s1->top;i++)
        if(s1->m[i] != s2->m[i])
            return 0;
    return 1;
}

int haveIntersection(Set *s1,Set *s2){
    int min = s1->top < s2->top ? s1->top : s2->top;
    for(int i=0;i<=min;i++)
        if((s1->m[i] & s2->m[i]) != 0)
            return 1;
    return 0;
}

int isEmptySet(Set *s){
    for(int i=0;i<=s->top;i++){
        if(s->m[i] != 0)
            return 0;
    }
    return 1;
}

void showSet(Set *s){
    Iter it = NewIter(s);
    printf("{");
    for(int i=Next(it);i != -1;i = Next(it))
        printf("%d ",i);
    printf("}");
    destroyIter(it);
}

int *setElement(Set *s){
    int *res = malloc(20 * sizeof (int));
    int *p = res;
    Iter it = NewIter(s);
    for(int i=Next(it);i != -1;i = Next(it))
        *p ++ = i;
    destroyIter(it);
    return res;
}
