#include<iostream>
#include"skiplist.h"
#include "random"
#include <chrono>
#include<string>
#include"SSTable.h"
#define MAX_SIZE 1000
int testlib[MAX_SIZE];
using namespace std;
bool testInsert(skiplist<uint64_t,string> *List){
    for(int i=0;i<MAX_SIZE;i++) List->insert(testlib[i],to_string(testlib[i]));
    return true;
}
bool testQuery(skiplist<uint64_t,string> *List){
    for(int i=0;i<MAX_SIZE;i++) {
        if(*List->get(testlib[i])!=to_string(testlib[i]))return false;}
    return true;
}
bool testRemove(skiplist<uint64_t,string> *List){
    for(int i=0;i<MAX_SIZE;i++) {
        if(!List->remove(testlib[i]))return false;}
    return true;
}
int main(){
    random_device rd;
    skiplist<uint64_t,string> list;
    string direc="./testData";
    SStable s(direc);
    for(int i=0;i<MAX_SIZE;i++)
    {
        auto value=(rd())%(2*MAX_SIZE);
        testlib[i]=value;
    }
    auto start = chrono::steady_clock::now();
    testInsert(&list);
    s.write(list);
    testRemove(&list);
    auto stop = chrono::steady_clock::now();
    auto duration=stop-start;
    cout<<chrono::duration<double, milli>(duration).count()
             << "ms ";
    vector<int> u;
  
    return 0;
}