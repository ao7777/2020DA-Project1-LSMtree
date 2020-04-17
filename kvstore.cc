#include "kvstore.h"
#include <string>
#include "skiplist.h"
#include<iostream>
KVStore::KVStore(const std::string &dir): KVStoreAPI(dir),dirc(dir),sstable(dirc)
{
	logFile.open(dirc+"/logfile.txt",std::fstream::binary|std::fstream::in|std::fstream::out);
	if(!logFile)
	logFile.open(dirc+"/logfile.txt",std::fstream::binary|std::fstream::in|std::fstream::trunc|std::fstream::out);
	readLog();
	logFile.close();
}

KVStore::~KVStore()
{
	
	std::remove((dirc+"/logfile.txt").c_str());
	logFile.close();
}
void KVStore::readLog(){
	logflag flag;
	key_t key;
	key_t length;
	logFile.seekp(std::ios::beg);
	while(logFile.read((char *)&flag,sizeof(logflag))){
		logFile.read((char *)&key,sizeof(key_t));
		logFile.read((char *)&length,sizeof(length));
		char * value=new char[length];
		logFile.read(value,length);
		if(flag)memtable.remove(key);
		else memtable.insert(key,value);
		delete [] value;
	}
}
/**
 * Insert/Update the key-value pair.
 * No return values for simplicity.
 */
void KVStore::put(uint64_t key, const std::string &s)
{
	
	memtable.insert(key,s);
	putLog(dataPair<uint64_t,std::string>(key,s));
	if(memtable.getSize()>maxMem){
		sstable.write(memtable);
		memtable=skiplist<key_t,value_t>();
		logFile.close();
		logFile.open(dirc+"/logfile.txt",std::fstream::binary|std::fstream::in|std::fstream::trunc|std::fstream::out);
	}
	
}
/**
 * Returns the (string) value of the given key.
 * An empty string indicates not found.
 */
std::string KVStore::get(uint64_t key)
{
	auto s=memtable.get(key);
	if(s)return *s;
	else{
		auto &&p=sstable.get(key);
		return p?*p:"";
	}
}
/**
 * Delete the given key-value pair if it exists.
 * Returns false iff the key is not found.
 */
bool KVStore::del(uint64_t key)
{
	if(memtable.remove(key)){
		putLog(dataPair<key_t,value_t>(key,"DELETED"),LOGDEL);
		return true;
	}
	return sstable.remove(key);
}

/**
 * This resets the kvstore. All key-value pairs should be removed,
 * including memtable and all sstables files.
 */
void KVStore::reset()
{
	memtable.~skiplist();
	sstable.clear();
}

void KVStore::putLog(dataPair<key_t,value_t> e,logflag flag){
	logFile.write((char *)&flag,sizeof(logflag));
	logFile.write((char *)&e.key,sizeof(key_t));
	auto length=e.value.size()+1;
	logFile.write((char*)&length,sizeof(length));
	logFile.write(e.value.c_str()+'\0',length);
}