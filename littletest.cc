#include<iostream>
#include"skiplist.h"
#include "random"
#define MAX_SIZE 100
int testlib[MAX_SIZE];
using namespace std;
bool testInsert(skiplist<int,int> *List){
    for(int i=0;i<MAX_SIZE;i++) List->insert(testlib[i],testlib[i]);
}
int main(){
    random_device rd;
    for(int i=0;i<MAX_SIZE;i++)
    {
        auto value=(rd())%(2*MAX_SIZE);
        testlib[i]=value;
    }
}