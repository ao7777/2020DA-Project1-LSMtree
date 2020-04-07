// this class is for storage in memory 
#include "random"
template<class key_t,class value_t>class skiplist{
public:
    skiplist();
    ~skiplist();
    int getSize();
    int getLevel();
    bool isEmpty();
    void insert(key_t,value_t);
    value_t* get(key_t);
    bool remove(key_t);
private:
    struct dataPair{
        value_t value;
        key_t key;
        dataPair(key_t k,value_t v):value(v),key(k){};
        dataPair(){};
    };
    struct qNode{//built-in quadListNode 
        qNode *above,*below,*next,*prev;
        dataPair entry;
        qNode(){};
        qNode(qNode *a,qNode *b,qNode *n,qNode *p,dataPair e):above(a),below(b),next(n),prev(p),entry(e){};
    };
    struct qList{
        qNode *head,*tail;
        qList *below,*above;
        int length;
        qList(){
            head=new qNode;
        head->next=tail=new qNode;
        tail->prev=head;
        head->prev=tail->next=nullptr;
        };
        ~qList(){
            auto p=head->next;
            while(p!=tail){
                p=p->next;
                delete p->prev;
            }
            delete head;
            delete tail;
        };
        bool isEmpty(){return head->next==tail;}
        qNode *first(){return head->next;}
        qNode *last(){return tail->prev;}
        bool isValid(qNode *p){return p&&(p!=head)&&(p!=tail);}
    };
    qList *levelTop,*levelBottom;//stores the list pointing to the first node of each level
    qNode* search(key_t);
    qNode* insertAbove(dataPair,qNode *,qNode *b=nullptr);
    int size,level;
};
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
    qList* tmp=levelTop;
    while(tmp->below){
        tmp=tmp->below;
        delete tmp->above;
    }
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
typename skiplist<key_t, value_t>::qNode *skiplist<key_t, value_t>::insertAbove(skiplist<key_t, value_t>::dataPair e, qNode *p, qNode *b)
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
        if(levelTop==levelBottom)break;
        auto tmp=levelTop->below;
        delete levelTop;
        levelTop=tmp;
    }
    size--;
    return true;
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