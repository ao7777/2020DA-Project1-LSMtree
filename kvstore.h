#pragma once
#include"skiplist.h"
#include "kvstore_api.h"
#include<fstream>
#include"SSTable.h"
typedef uint64_t key_t;
typedef std::string value_t;
class KVStore : public KVStoreAPI {
	// You can add your implementation here
private:
	enum logflag{
		LOGPUT,
		LOGDEL
	};
	const static int maxMem=(1<<9);
	std::fstream logFile;
	std::string dirc;
	skiplist<key_t,value_t> memtable;
	SStable sstable;
	void putLog(dataPair<key_t,value_t>,logflag flag=LOGPUT);
	void readLog();
public:
	KVStore(const std::string &dir);

	~KVStore();

	void put(uint64_t key, const std::string &s) override;

	std::string get(uint64_t key) override;

	bool del(uint64_t key) override;

	void reset() override;

};
