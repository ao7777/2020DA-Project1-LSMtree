// this class is for storage in memory 
#pragma once
#include "random"
#include<memory>

template<class key_t,class value_t>class skiplist{
public:
    skiplist();
    ~skiplist();
    int getSize();
    int getLevel();
    bool isEmpty();
    void insert(key_t,value_t); 
    std::shared_ptr<value_t> get(key_t);
    bool remove(key_t);
private:
    struct dataPair{
        value_t value;
        key_t key;
        dataPair(key_t k,value_t v):value(v),key(k){};
        dataPair(){};
    };
    struct qNode{//built-in quadListNode 
        std::shared_ptr<qNode> above,below,next,prev;
        dataPair entry;
        qNode(){};
        qNode(std::shared_ptr<qNode> a,
        std::shared_ptr<qNode>b,
        std::shared_ptr<qNode>n,
        std::shared_ptr<qNode>p,
        dataPair e):above(a),below(b),next(n),prev(p),entry(e){};
    };
    struct qList{
        std::shared_ptr<qNode> head,tail;
        std::shared_ptr<qList> below,above;
        int length;
        qList(){
        head=std::make_shared<qNode>();
        head->next=tail=std::make_shared<qNode>();
        tail->prev=head;
        head->prev=tail->next=nullptr;
        };
        ~qList(){
        };
        bool isEmpty(){return head->next==tail;}
        std::shared_ptr<qNode>first(){return head->next;}
        std::shared_ptr<qNode>last(){return tail->prev;}
        bool isValid(std::shared_ptr<qNode> p){return p&&(p!=head)&&(p!=tail);}
    };
   std::shared_ptr<qList> levelTop,levelBottom;//stores the list pointing to the first node of each level
   std::shared_ptr<qNode> search(key_t);
   std::shared_ptr<qNode> insertAbove(dataPair,std::shared_ptr<qNode>,std::shared_ptr<qNode> b=nullptr);
    int size,level;
};
template <class key_t, class value_t>
skiplist<key_t, value_t>::skiplist()
{
    size = level = 0;
    levelTop = std::make_shared<qList>();
    levelTop->below=levelBottom=std::make_shared<qList>();
    levelBottom->below=levelTop->above = nullptr;
    levelBottom->above=levelTop;
};
template <class key_t, class value_t>
skiplist<key_t, value_t>::~skiplist()
{
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
    std::shared_ptr<qNode> p, b;
    p = search(key);
    if (p->entry.key == key)
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
                    levelTop->above=std::make_shared<qList>();;
                    levelTop->above->below=levelTop;
                    levelTop=levelTop->above;
                    this->level++;
                }
                p=level->above->head;
            }
            else p=p->above;
            level=level->above;
            b=insertAbove(e,p,b); 
    }
}
template <class key_t, class value_t>
std::shared_ptr<typename skiplist<key_t, value_t>::qNode> skiplist<key_t, value_t>::insertAbove(skiplist<key_t, value_t>::dataPair e, std::shared_ptr<skiplist<key_t, value_t>::qNode>p, std::shared_ptr<skiplist<key_t, value_t>::qNode>b)
{
    //insert a node before p
    std::shared_ptr<qNode> x = std::shared_ptr<qNode>(new qNode(nullptr, b, p->next, p, e));
    p->next->prev=x;
    p->next=x;
    if(b)b->above=x;
    return x;
}
template <class key_t, class value_t>
std::shared_ptr<value_t> skiplist<key_t, value_t>::get(key_t key)
{
    auto p=search(key);
    return (p->entry.key==key)?std::make_shared<value_t>(p->entry.value):nullptr;
}
template <class key_t, class value_t>
bool skiplist<key_t, value_t>::remove(key_t key)
{
    if(isEmpty())return false;
    auto p=search(key);
    if(p->entry.key!=key)return false;
    do{
        std::shared_ptr<qNode> tmp=p->below;
        p->prev->next=p->next;
        p->next->prev=p->prev;
        p=tmp;
    }while(p);
    while(!isEmpty()&&levelTop->isEmpty()){
        if(levelTop==levelBottom)break;
        auto tmp=levelTop->below;
        levelTop=tmp;
    }
    size--;
    return true;
}
template <class key_t, class value_t>
std::shared_ptr<typename skiplist<key_t, value_t>::qNode> skiplist<key_t, value_t>::search(key_t key)
{
    //search from the first level
    auto topList = levelTop;
    auto p = topList->first();
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