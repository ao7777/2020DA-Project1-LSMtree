#include<iostream>
#include"skiplist.h"
#include "random"
#define MAX_SIZE 50000
int testlib[MAX_SIZE];
using namespace std;
bool testInsert(skiplist<int,int> *List){
    for(int i=0;i<MAX_SIZE;i++) List->insert(testlib[i],testlib[i]);
    return true;
}
bool testQuery(skiplist<int,int> *List){
    for(int i=0;i<MAX_SIZE;i++) {
        if(*List->get(testlib[i])!=testlib[i])return false;}
    return true;
}
bool testRemove(skiplist<int,int> *List){
    for(int i=0;i<MAX_SIZE;i++) {
        if(!List->remove(testlib[i]))return false;}
    return true;
}
int main(){
    random_device rd;
    skiplist<int,int> list;
    for(int i=0;i<MAX_SIZE;i++)
    {
        auto value=(rd())%(2*MAX_SIZE);
        testlib[i]=value;
    }
    testInsert(&list);
    cout<<testRemove(&list);
    return 0;
}