#pragma once
#include<fstream>
#include<memory>
#include"skiplist.h"
#include<stack>
typedef uint64_t key_t;
typedef std::string value_t;
class SStable{
public:
    SStable()=delete;
    SStable(std::string &dir);
    ~SStable();
    void write(skiplist<key_t,value_t> &s);
    void clear();
    void initialize();
    bool remove(key_t);
    std::shared_ptr<value_t> get(key_t);
private: 
    const static int MAXFILESIZE=(1<<14);
    void merge(int);
    skiplist<key_t,std::streamoff> mergeWrite(int,int,int);
    std::string directory;
    std::vector<key_t> levels;//stores the number of file on each level
    std::vector< skiplist<key_t,std::streamoff> > indexCache;
    int pos(int i,int j){
        int posi=0;
        while(i>0){
            posi=posi*10+4;
            i--;
        }
        return posi+j+(i!=0);
    }
    int headerPos;
    std::fstream file,hfile;
};
