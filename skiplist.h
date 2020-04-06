// this class is for storage in memory 
#pragma once 
#include <list>
template<class key_t,class value_t>class skiplist{
public:
skiplist();
~skiplist();
int getSize();
int getLevel();
void insert(key_t,value_t);
value_t* get(key_t);
bool remove(key_t);
private:
    struct qNode{//built-in quadListNode 
        qNode *above,*below,*front,*behind;
        key_t key;
    };
    qNode* search(key_t);
    list<qNode> * qList;
    int size,level;
};