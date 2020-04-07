#include "skiplist.h"
#include <random>
template <class key_t, class value_t>
skiplist<key_t, value_t>::skiplist()
{
    size = level = 0;
    levelTop = new qList;
    levelTop->below=levelBottom=new qList;
    levelBottom->below=levelTop->above = nullptr;
    levelBottom->above=levelTop;
};
template <class key_t, class value_t>
skiplist<key_t, value_t>::~skiplist()
{
    qList tmp=levelTop;
    while(tmp->below){
        tmp=tmp->below;
        tmp->above->~qList();
        delete tmp->above;
    }
    tmp->~qList();
    delete tmp;
}
template <class key_t, class value_t>
bool skiplist<key_t, value_t>::isEmpty()
{
    return size == 0;
}
template <class key_t, class value_t>
int skiplist<key_t, value_t>::getSize()
{
    return size;
}
template <class key_t, class value_t>
int skiplist<key_t, value_t>::getLevel()
{
    return level;
}
template <class key_t, class value_t>
void skiplist<key_t, value_t>::insert(key_t key, value_t value)
{
    dataPair e = dataPair(key, value);
    qNode *p, *b;
    p = search(key);
    if (p.entry.key == key)
        while (p->below)
            p = p->below;
    size++;
    b = insertAbove(e, p);
    std::random_device rd;
    auto level=levelBottom;
    while (rd() & 1)
    {
        while (level->isValid(p) && (!p->above))
            p = p->prev;
            if(!level->isValid(p)){
                if(level==levelTop){
                    levelTop->above=new qList;
                    levelTop->above->below=levelTop;
                    levelTop=levelTop->above;
                    level++;
                }
                p=level->above->head;
            }
            else p=p->above;
            level=level->above;
            b=insertAbove(e,p,b); 
    }
}
template <class key_t, class value_t>
typename skiplist<key_t, value_t>::qNode *skiplist<key_t, value_t>::insertAbove(skiplist<key_t, value_t>::dataPair e, qNode *p, qNode *b = nullptr)
{
    //insert a node before p
    qNode *x = new qNode(nullptr, b, p->next, p, e);
    p->next->prev=x;
    p->next=x;
    if(b)b->above=x;
    return x;
}
template <class key_t, class value_t>
value_t *skiplist<key_t, value_t>::get(key_t key)
{
    auto p=search(key);
    return (p->entry.key==key)?&p->entry.value:nullptr;
}
template <class key_t, class value_t>
bool skiplist<key_t, value_t>::remove(key_t key)
{
    if(isEmpty())return false;
    auto p=search(key);
    if(p->entry.key!=key)return false;
    do{
        qNode *tmp=p->below;
        p->prev->next=p->next;
        p->next->prev=p->prev;
        delete p;
        p=tmp;
    }while(p);
    while(!isEmpty()&&levelTop->isEmpty()){
        auto tmp=levelTop->below;
        delete levelTop;
        levelTop=tmp;
    }
}
template <class key_t, class value_t>
typename skiplist<key_t, value_t>::qNode *skiplist<key_t, value_t>::search(key_t key)
{
    //search from the first level
    auto topList = levelTop;
    qNode *p = topList->first();
    while (1)
    {
        while (p->next && (p->entry.key <= key))
            p = p->next;
        p = p->prev;
        if (p->prev && (key == p->entry.key))
            return p;
        topList = topList->below;
        if (!topList)
            return p;
        p = p->prev ? p->below : topList->first();
    }
}