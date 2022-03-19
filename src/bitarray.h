//
// Created by 谢卫凯 on 2022/3/17.
// 提供了集合操作（当然是正整数集）
// 基于bit数组实现
// 提供的各项操作详见下面的函数声明
//

#ifndef LAB1_BITARRAY_H
#define LAB1_BITARRAY_H

typedef struct{
    unsigned int *m;  //分配内存用的指针
    int top; //最大的不为0的一个位置 即m[top]不等于0 对于任意 i>top m[top]==0
    int size; //m的元素个数
}Set;

typedef struct {
    Set *s;
    int cur;
}*Iter,iter;

//return a new empty set
Set *NewSet();

//add an int element e in the set
void Add(Set *s,int e);

//remove element e from the set
void Remove(Set *s,int e);

//union opteration
void Union(Set *s1,Set *s2);

//check if element e in the set
int In(Set *s,int e);

//check if two sets are the same
int SameSet(Set *s1,Set *s2);

//destroy a set
void destroy(Set *s);

//return iterator of set s
Iter NewIter(Set *s);

//get next element in set s
//return -1 if no element left
//notice that when using iterator,don't operate on the set
int Next(Iter it);

//destroy an iterator
void destroyIter(Iter it);

//check if two sets have intersection
int haveIntersection(Set *s1,Set *s2);

//check if a set is empty
int isEmptySet(Set *s);

//下面这俩是当初debug时用的
void showSet(Set *s);
//
int *setElement(Set *s);

#endif //LAB1_BITARRAY_H
