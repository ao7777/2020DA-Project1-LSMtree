#include<iostream>
#include"skiplist.h"
#include "random"
#include <chrono>
#define MAX_SIZE 10000
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
    auto start = chrono::steady_clock::now();
    testInsert(&list);
    testRemove(&list);
    auto stop = chrono::steady_clock::now();
    auto duration=stop-start;
    cout<<chrono::duration<double, milli>(duration).count()
             << "ms ";
    return 0;
}