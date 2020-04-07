// this class is for storage in memory 
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
    qNode* insertAbove(dataPair,qNode *,qNode *);
    int size,level;
};